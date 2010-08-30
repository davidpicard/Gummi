#include <stdlib.h>

#include "environment.h"
#include "motion.h"

/* reference to global environment instance */
extern gummi_t* gummi;

motion_t* motion_init(gint dum) {
    motion_t* m = (motion_t*)malloc(sizeof(motion_t));
    m->dummy = dum;
    return m;
}

void update_preview() {
    update_workfile();
}


void update_workfile() {
       printf("o hai\n"); 
}


/*

    def update_workfile(self):
        try:
            # these two lines make the program hang in certain situations
            #self.editorpane.editorview.set_sensitive(False)
            buff = self.editorpane.editorviewer.get_buffer()
            start_iter, end_iter = buff.get_start_iter(), buff.get_end_iter()
            content = buff.get_text(start_iter, end_iter)
            #self.editorpane.editorview.set_sensitive(True)
            tmpmake = open(self.workfile, "w")
            tmpmake.write(content)
            tmpmake.close()
            self.editorviewer.grab_focus() #editorpane regrabs focus
        except:
            print traceback.print_exc()
*/


//          if self.previewpane and self.editorpane.check_buffer_changed():
//              self.editorpane.check_buffer_changed()
    //          self.update_workfile()
            //  retcode = self.update_pdffile()
        //      self.update_errortags(retcode)
        //      self.cleanup_fd(15)
            //  self.previewpane.refresh_preview()

