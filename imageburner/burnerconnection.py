import serial
from imageburner.burnerprotocol import InquiryReq, StartPieceReq, ImageDataReq


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
        self._max_piece_size = 512

    @property
    def max_dim(self):
        return self._max_dim

    def _send_image_piece(self, x, y, width, height, img_data):
        total_entries_to_send = width * height
        assert total_entries_to_send == len(img_data)
        assert total_entries_to_send <= self._max_piece_size
        assert x + width < self._max_dim
        assert y + height < self._max_dim
        start_req = StartPieceReq(self._serial, x, y, width, height)
        ack = start_req.send()
        if ack.status != 0:
            raise Exception('Error %u starting image piece' % ack.status)
        entries_sent = 0
        while entries_sent < total_entries_to_send:
            entries_to_send = min(total_entries_to_send - entries_sent, self._rx_buffer_size)
            img_piece = img_data[entries_sent:entries_sent + entries_to_send]
            img_data_req = ImageDataReq(self._serial, len(img_piece), 0)   # TODO: set CRC
            ack = img_data_req.send(img_piece)
            if ack.status != 0:
                raise Exception('Error %u sending image data' % ack.status)
            entries_sent += entries_to_send
            assert entries_sent <= total_entries_to_send
            if entries_sent == total_entries_to_send:
                if not ack.complete:
                    raise Exception('Burner did not receive all of data sent!')

    def xfer_image(self, img):
        if img.size[0] > self._max_dim or img.size[1] > self._max_dim:
            raise Exception('image (%ux%u) is too big. Must not be larger than %ux%u' % (img.size[0], img.size[1], self._max_dim, self._max_dim))
        total_image_entries = img.size[0] * img.size[1]
        entries_xferred = 0
        while entries_xferred < total_image_entries:
            start_y = entries_xferred / img.size[0]
            start_x = entries_xferred % img.size[0]
            # transfer as much of this row as we can
            # This is not very efficient, but we can change that later
            entries_to_send = min(img.size[0] - start_x, self._max_piece_size)
            img_data = bytearray(entries_to_send)
            for x in range(start_x, start_x + entries_to_send):
                b = img.getpixel((x, start_y))
                img_data[x - start_x] = b
            self._send_image_piece(start_x, start_y, entries_to_send, 1, img_data)
            

