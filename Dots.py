
import AcceleratedDots
import numpy as np
import tkinter as tk
import PIL.Image, PIL.ImageTk
import cv2


# Config
#Make sure the screen width and height match with the C program
screen_width = 400
screen_height = 800
dots_count = 400
min_size = 5.
max_size = 10.
min_velocity = -2
max_velocity = 2.
seed = 0

numpy_array = np.ones((screen_height, screen_width, 3), 
                                        dtype=np.uint8)


# Initializing my Dots
AcceleratedDots.create_dots(numpy_array,
    screen_width,
    screen_height,
    dots_count, 
    seed,
    min_velocity,
    max_velocity, 
    min_size,
    max_size)





# Initializing Tkinter


#img = PIL.Image.fromarray(numpy_array)
#img.show()

# cv2.imwrite("RyansBG.png", numpy_array)

AcceleratedDots.request_image()

root = tk.Tk()

pilimg = PIL.Image.fromarray(numpy_array, mode="RGB")
img = PIL.ImageTk.PhotoImage(pilimg)
label = tk.Label(root, image=img)
label.image = img
label.pack()
root.update()

frames = 5000
for x in range(1, frames):
    AcceleratedDots.request_image()

    next_img = PIL.ImageTk.PhotoImage(
        PIL.Image.fromarray(numpy_array, mode="RGB"))
    
    root.title(str(x))

    label.configure(image=next_img)
    label.image = next_img
    root.update()



AcceleratedDots.uncreate_dots()

print("Looped")
root.mainloop()