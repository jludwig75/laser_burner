import serial


BURNER_MAGIC = (0x46, 0x52) # 'FR' big endian
REGISTER_REQ_OP = 0
REGISTER_ACK_OP = 1
START_PIECE_REQ_OP = 2
START_PIECE_ACK_OP = 3
IMG_DATA_REQ_OP = 4
IMG_DATA_ACK_OP = 5
COMPLETE_PIECE_ACK_OP = 6


class RegisterAck:
    def __init__(self, serial):
        self._serial = serial

    def receive(self):
        bytes = bytearray(8)
        bytes = self._serial.read(len(bytes))
        if bytes[0] != BURNER_MAGIC[0] or bytes[1] != BURNER_MAGIC[1]:
            raise Exception('Bad burner magic')
        op = (bytes[2] << 8) | bytes[3]
        if op != REGISTER_ACK_OP:
            raise Exception('Expected register ack op %u, got op %u' % (REGISTER_ACK_OP, op))
        self.rx_buffer_size = (bytes[4] << 8) | bytes[5]
        self.max_dim = (bytes[6] << 8) | bytes[7]

class RegisterReq:
    def __init__(self, serial):
        self._serial = serial

    def send(self):
        self._serial.write(self._req_bytes)
        ack = RegisterAck(self._serial)
        ack.receive()
        return ack

    @property
    def _req_bytes(self):
        # encode in net order (big endian)
        bytes = bytearray(4)
        bytes[0] = BURNER_MAGIC[0]
        bytes[1] = BURNER_MAGIC[1]
        bytes[2] = 0
        bytes[3] = 0
        return bytes


class BurnerConnection:
    def __init__(self, port_device):
        print('Connecting to serial device "%s"' % port_device)
        self._serial = serial.Serial(port_device, 9600)
        self._register()

    def _register(self):
        print('Registering with burner...')
        # 1. Send registration packet
        reg_request = RegisterReq(self._serial)
        # 2. Get registration response (device RX buffer size, max dimension)
        ack = reg_request.send()
        print('Successfully segistered with burner')
        self._rx_buffer_size = ack.rx_buffer_size
        self._max_dim = ack.max_dim

    @property
    def max_dim(self):
        return self._max_dim

    def xfer_image(self, img):
        pass