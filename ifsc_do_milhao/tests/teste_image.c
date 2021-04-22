#include <allegro5/allegro.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>

#define BACKGROUND_FILE "boot.png"
int main(void){

    ALLEGRO_DISPLAY *display = NULL;
    ALLEGRO_BITMAP  *boot=NULL;
    if(!al_init()) {
        fprintf(stderr, "failed to initialize allegro!\n");
        return -1;
    }
    al_init_image_addon();//should check for errors

    display = al_create_display(640, 480);
    if(!display) {
        fprintf(stderr, "failed to create display!\n");
        return -1;
    }

    boot=al_load_bitmap("/Users/pamela_fialho/Documents/GitHub/ifsc_do_milhao/ifsc_do_milhao/boot.png");
    if(!boot)
    {
        fprintf(stderr, "failed to load background bitmap!\n");
        return -1;
    }
    al_draw_bitmap(boot,0,0,0);

    al_flip_display();

    al_rest(2.0);

    al_destroy_display(display);
    al_destroy_bitmap(boot);
    al_uninstall_system();
    return 0;
}
