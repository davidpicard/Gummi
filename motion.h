#ifndef GUMMI_MOTION_H
#define GUMMI_MOTION_H

typedef struct _motion {
    gint dummy; // you can add members along the way
} motion_t;

motion_t* motion_init(gint dum);
void update_preview();
void update_workfile();

#endif /* GUMMI_MOTION_H */

