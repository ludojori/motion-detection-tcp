#ifndef CAMERA_H
#define CAMERA_H
/**
    A mock camera driver
*/


/**
Returns the resolution of the camera
    @param width the addres of the variable that the width is going to be written to
    @param height the addres of the variable that the height is going to be written to
*/
void getResolution(int *width, int *height);

/**
Returns the current image from the camera
    @param pixels buffer in which the pixels are going to be written to
    each unsigned int is a single pixels RGBA (red, green, blue, alpha) 1 byte for each color
    alpha is always 255
*/
void getCurrentImage(unsigned int *pixels);

/**
Generates a fake movement
*/
void generateMovement();

#endif /* CAMERA_H */