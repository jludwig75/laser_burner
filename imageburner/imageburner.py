from PIL import Image


def replace_transparent(img):
    if not 'A' in img.mode.upper():
        return img
    new_img = Image.new(img.mode, img.size, "WHITE")
    new_img.paste(img, (0,0), img)
    return new_img


class ImageBurner:
    def __init__(self, port_device):
        self._port_device = port_device 

    def init(self):
        # TODO: Get from serial ifc
        # TODO: Do we want to not enforce a square device?
        self._max_dim = 150

    def _gen_burn_image(self, img, output_image_file_name):
        print(img.mode)
        max_dim = max(img.size)
        # Reize to fit smaller or bigger TODO: add an option to not resize larger
        new_size = (self._max_dim * img.size[0] // max_dim, self._max_dim * img.size[1] // max_dim)
        img = img.resize(new_size, Image.LANCZOS) # Use high quality. We have time
        img = replace_transparent(img)
        # Reduce color depth to 8-bit grayscale
        img = img.convert('L')
        if output_image_file_name != None:
            img.save(output_image_file_name)
        return img

    def _xfer_img(self, img):
        pass

    def burn_image(self, image_file_name, output_image_file_name = None):
        img = Image.open(image_file_name)
        img = self._gen_burn_image(img, output_image_file_name)
        self._xfer_img(img)
