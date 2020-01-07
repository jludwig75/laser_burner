from PIL import Image
from imageburner.imageformatter import ImageFormatter
from imageburner.burnerconnection import BurnerConnection


class ImageBurner:
    def __init__(self, port_device):
        self._con = BurnerConnection(port_device)
        self._fomatter = ImageFormatter(self._con.max_dim)

    def burn_image(self, image_file_name, output_image_file_name = None):
        img = self._fomatter.gen_burn_image(image_file_name)
        if output_image_file_name != None:
            img.save(output_image_file_name)
        self._con.xfer_image(img)
