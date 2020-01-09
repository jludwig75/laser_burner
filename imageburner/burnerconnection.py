import serial


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


class Ack:
    def __init__(self, serial):
        self._serial = serial

    def receive(self):
        bytes = self._serial.read(6)
        if bytes[0] != BURNER_MAGIC[0] or bytes[1] != BURNER_MAGIC[1]:
            raise Exception('Bad burner magic')
        op = (bytes[2] << 8) | bytes[3]
        status = (bytes[4] << 8) | bytes[5]
        return (op, status)

class InquiryAck(Ack):
    def __init__(self, serial):
        Ack.__init__(self, serial)
        self._serial = serial

    def receive(self):
        op, status = Ack.receive(self)
        if op != INQUIRY_ACK_OP:
            raise Exception('Expected inquiry op (%u), got op %u' % (INQUIRY_ACK_OP, op))
        if status != ACK_SATUS_SUCCESS:
            raise Exception('Failed inquiry ACK: error status %u' % status)
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


class BurnerConnection:
    def __init__(self, port_device):
        print('Connecting to serial device "%s"' % port_device)
        self._serial = serial.Serial(port_device, 9600)
        self._inquiry()

    def _inquiry(self):
        print('Inquirying burner parameters...')
        # 1. Send inquiry packet
        reg_request = InquiryReq(self._serial)
        # 2. Get inquiry response (device RX buffer size, max dimension)
        ack = reg_request.send()
        print('Successfully iquiried burner parameters: rx_buffer_size=%u, max_dim=%u' % (ack.rx_buffer_size, ack.max_dim))
        self._rx_buffer_size = ack.rx_buffer_size
        self._max_dim = ack.max_dim

    @property
    def max_dim(self):
        return self._max_dim

    def xfer_image(self, img):
        pass