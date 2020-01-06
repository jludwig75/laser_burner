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

    def _resize_image_to_fit(self, img):
        max_dim = max(img.size)
        # 1. Reize to fit smaller or bigger TODO: add an option to not resize larger
        new_size = (self._max_dim * img.size[0] // max_dim, self._max_dim * img.size[1] // max_dim)
        return img.resize(new_size, Image.LANCZOS) # Use high quality. We have time

    def _convert_img_to_grayscale(self, img):
        img = replace_transparent(img)
        # 2. Reduce color depth to 8-bit grayscale
        return img.convert('L')

    def _pad_image_to_full_frame(self, img):
        # 3. pad the image out to the full size. Center it for now.
        # Either the width or the height must match the max size (for now)
        assert img.width == self._max_dim or img.height == self._max_dim
        if img.width < self._max_dim or img.height < self._max_dim:
            new_img = Image.new(img.mode, (self._max_dim, self._max_dim), "WHITE")
            x = (new_img.width - img.width) // 2
            y = (new_img.height - img.height) // 2
            new_img.paste(img, (x, y))
            img = new_img
        return img

    def _gen_burn_image(self, img, output_image_file_name):
        img = self._resize_image_to_fit(img)
        img = self._convert_img_to_grayscale(img)
        img = self._pad_image_to_full_frame(img)
        if output_image_file_name != None:
            img.save(output_image_file_name)
        return img

    def _xfer_img(self, img):
        pass

    def burn_image(self, image_file_name, output_image_file_name = None):
        img = Image.open(image_file_name)
        img = self._gen_burn_image(img, output_image_file_name)
        self._xfer_img(img)
