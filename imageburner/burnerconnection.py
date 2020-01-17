import serial
from imageburner.burnerprotocol import InquiryReq


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