from PIL import Image
import math

def diagonal_distance(x, y):
    return y * 16 + x
    # return int(math.sqrt(x * x + y * y))

img = Image.new('L', (16, 16), "WHITE")

max_value = img.size[0] * img.size[1]

for y in range(img.size[1]):
    for x in range(img.size[0]):
        # print('(%u, %u)' % (x, y))
        distance = diagonal_distance(x, y) + 1
        # print(distance)
        value = (255 * distance) // max_value
        img.putpixel((x, y), value)

img.save('gradient.pgm')