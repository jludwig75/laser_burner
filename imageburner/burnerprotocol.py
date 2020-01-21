BURNER_MAGIC = (0x46, 0x52) # 'FR' big endian

# OP Codes
UNKNOWN_OP = 0
INQUIRY_REQ_OP = 1
INQUIRY_ACK_OP = 2
START_PIECE_REQ_OP = 3
START_PIECE_ACK_OP = 4
IMG_DATA_REQ_OP = 5
IMG_DATA_ACK_OP = 6
COMPLETE_PIECE_ACK_OP = 7

# Status codes
ACK_SATUS_SUCCESS = 0
ACK_SATUS_BAD_MAGIC = 1
ACK_SATUS_INVALID_REQUEST_OP = 2
ACK_SATUS_INVALID_REQUEST = 3
ACK_SATUS_INVALID_PARAMETER = 4
ACK_SATUS_NOT_IMPLEMENTED = 5
ACK_STATUS_IO_ERROR = 6

def U16_HI_BYTE(x):
    return x >> 8

def U16_LO_BYTE(x):
    return x & 0xFF

class Ack:
    def __init__(self, serial):
        self._serial = serial

    def receive(self):
        bytes = self._serial.read(6)
        if bytes[0] != BURNER_MAGIC[0] or bytes[1] != BURNER_MAGIC[1]:
            raise Exception('Bad burner magic')
        self.op = (bytes[2] << 8) | bytes[3]
        self.status = (bytes[4] << 8) | bytes[5]
        return (self.op, self.status)

class UnknownAck(Ack):
    def __init__(self, serial):
        Ack.__init__(self, serial)
        self._serial = serial

    def receive(self):
        op, status = Ack.receive(self)
        if status != ACK_SATUS_SUCCESS:
            raise Exception('Failed inquiry ACK: error status %u' % status)
        if op != INQUIRY_ACK_OP:
            raise Exception('Expected inquiry op (%u), got op %u' % (INQUIRY_ACK_OP, op))

class UnknownReq:
    def __init__(self, serial, opcode):
        self._serial = serial
        self._opcode = opcode

    def send(self):
        self._serial.write(self._req_bytes)
        ack = UnknownAck(self._serial)
        ack.receive()
        return ack

    @property
    def _req_bytes(self):
        # encode in net order (big endian)
        bytes = bytearray(4)
        bytes[0] = BURNER_MAGIC[0]
        bytes[1] = BURNER_MAGIC[1]
        bytes[2] = U16_HI_BYTE(self._opcode)
        bytes[3] = U16_LO_BYTE(self._opcode)
        return bytes

class InquiryAck(Ack):
    def __init__(self, serial):
        Ack.__init__(self, serial)
        self._serial = serial

    def receive(self):
        op, status = Ack.receive(self)
        if status != ACK_SATUS_SUCCESS:
            raise Exception('Failed inquiry ACK: error status %u' % status)
        if op != INQUIRY_ACK_OP:
            raise Exception('Expected inquiry op (%u), got op %u' % (INQUIRY_ACK_OP, op))
        bytes = self._serial.read(4)
        self.rx_buffer_size = (bytes[0] << 8) | bytes[1]
        self.max_dim = (bytes[2] << 8) | bytes[3]

class InquiryReq:
    def __init__(self, serial):
        self._serial = serial

    def send(self):
        self._serial.write(self._req_bytes)
        ack = InquiryAck(self._serial)
        ack.receive()
        return ack

    @property
    def _req_bytes(self):
        # encode in net order (big endian)
        bytes = bytearray(4)
        bytes[0] = BURNER_MAGIC[0]
        bytes[1] = BURNER_MAGIC[1]
        bytes[2] = 0
        bytes[3] = INQUIRY_REQ_OP
        return bytes

class StartPieceAck(Ack):
    def __init__(self, serial):
        Ack.__init__(self, serial)
        self._serial = serial

    def receive(self):
        op, status = Ack.receive(self)
        if status != ACK_SATUS_SUCCESS:
            raise Exception('Failed start piece ACK: error status %u' % status)
        if op != START_PIECE_ACK_OP:
            raise Exception('Expected start piece op (%u), got op %u' % (START_PIECE_ACK_OP, op))

class StartPieceReq:
    def __init__(self, serial, start_x, start_y, width, height):
        self._serial = serial
        self.start_x = start_x
        self.start_y = start_y
        self.width = width
        self.height = height

    def send(self):
        self._serial.write(self._req_bytes)
        ack = StartPieceAck(self._serial)
        ack.receive()
        return ack

    @property
    def _req_bytes(self):
        # encode in net order (big endian)
        bytes = bytearray(12)
        bytes[0] = BURNER_MAGIC[0]
        bytes[1] = BURNER_MAGIC[1]

        # network (big endian) byte order
        bytes[2] = 0
        bytes[3] = START_PIECE_REQ_OP

        bytes[4] = U16_HI_BYTE(self.start_x)
        bytes[5] = U16_LO_BYTE(self.start_x)

        bytes[6] = U16_HI_BYTE(self.start_y)
        bytes[7] = U16_LO_BYTE(self.start_y)

        bytes[8] = U16_HI_BYTE(self.width)
        bytes[9] = U16_LO_BYTE(self.width)

        bytes[10] = U16_HI_BYTE(self.height)
        bytes[11] = U16_LO_BYTE(self.height)

        return bytes

class ImageDataAck(Ack):
    def __init__(self, serial):
        Ack.__init__(self, serial)
        self._serial = serial

    def receive(self):
        op, status = Ack.receive(self)
        if status != ACK_SATUS_SUCCESS:
            raise Exception('Failed start piece ACK: error status %u' % status)
        if op != IMG_DATA_ACK_OP:
            raise Exception('Expected start piece op (%u), got op %u' % (IMG_DATA_ACK_OP, op))
        bytes = self._serial.read(1)
        self.complete = False if bytes[0] == 0 else True

def dump_bytes(bytes):
    st = ''
    for b in bytes:
        st += hex(int(b)) + ','
    return st


class ImageDataReq:
    def __init__(self, serial, number_of_bytes, image_data_crc):
        self._serial = serial
        self.number_of_bytes = number_of_bytes
        self.image_data_crc = image_data_crc

    def send(self, image_data):
        self._serial.write(self._req_bytes)
        self._serial.write(image_data)
        ack = ImageDataAck(self._serial)
        ack.receive()
        return ack

    @property
    def _req_bytes(self):
        # encode in net order (big endian)
        bytes = bytearray(8)
        bytes[0] = BURNER_MAGIC[0]
        bytes[1] = BURNER_MAGIC[1]

        # network (big endian) byte order
        bytes[2] = 0
        bytes[3] = IMG_DATA_REQ_OP

        bytes[4] = U16_HI_BYTE(self.number_of_bytes)
        bytes[5] = U16_LO_BYTE(self.number_of_bytes)

        bytes[6] = U16_HI_BYTE(self.image_data_crc)
        bytes[7] = U16_LO_BYTE(self.image_data_crc)

        return bytes
