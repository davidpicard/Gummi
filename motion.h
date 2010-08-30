#ifndef GUMMI_MOTION_H
#define GUMMI_MOTION_H

typedef struct _motion {
    gint dummy; // you can add members along the way
} motion_t;

motion_t* motion_init(gint dum);
void initial_preview();
void update_workfile();
void update_pdffile();


#endif /* GUMMI_MOTION_H */

