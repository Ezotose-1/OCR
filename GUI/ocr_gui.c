#include <gtk/gtk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include "imageProcessing.h"


    // Global variables
static GtkWidget *imagePath;
static GtkWidget *image;
static GtkWidget *TrainingCheck;
static GdkPixbuf *pixbuf;

void ImageReload(){
    printf("[OCR] - Image Reloaded\n");
    const char* path = gtk_label_get_text( GTK_LABEL(imagePath) );
    pixbuf = gdk_pixbuf_new_from_file (path, NULL);
    gtk_image_set_from_pixbuf(GTK_IMAGE(image), pixbuf);
    gdk_display_flush(gdk_display_get_default());
}


        // Basic window structure
void destroy(GtkWidget* widget, gpointer data)
{
    g_print("[OCR] - Application closed\n");
    gtk_main_quit();
}

        // Secondary functions
//-------------------RUN-ALL-PRE-TREATEMENT------------------//
static void RunImageProcessing () {
    const char* path = gtk_label_get_text( GTK_LABEL(imagePath) );
    gdk_pixbuf_save(pixbuf, path, "bmp", NULL, NULL);    
    if (!pixbuf) {
        g_print("[OCR] No picture imported\n");
    }
    else {
        process( path , 5);
        printf("[OCR] - Training mode  : %s\n", gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (TrainingCheck)) ? "Yes" : "No");
    }
    ImageReload();
}
//-----------------------------------------------------------//




//-------------------PRE-TREATEMENT-BUTTON-------------------//
static void GrayScaleProcess()
{
    const char* path = gtk_label_get_text( GTK_LABEL(imagePath) );
    gdk_pixbuf_save(pixbuf, path, "bmp", NULL, NULL);    
    if (!pixbuf) {
        g_print("[OCR] No picture imported\n");
    }
    else
        process( path , 0);
    ImageReload();
}

static void ContrastProcess()
{
    const char* path = gtk_label_get_text( GTK_LABEL(imagePath) );
    gdk_pixbuf_save(pixbuf, path, "bmp", NULL, NULL);    
    if (!pixbuf) {
        g_print("[OCR] No picture imported\n");
    }
    else
        process( path , 1);
    ImageReload();
}

static void CharacterFinderProcess()
{
    const char* path = gtk_label_get_text( GTK_LABEL(imagePath) );
    gdk_pixbuf_save(pixbuf, path, "bmp", NULL, NULL);    
    if (!pixbuf) {
        g_print("[OCR] No picture imported\n");
    }
    else
        process( path , 2);
    ImageReload();
}
//-----------------------------------------------------------//


//--------------------ROTATIONS-BUTTONS----------------------//
static void LeftRotate_pixbuf () {

    if (pixbuf != NULL) {
        pixbuf = gdk_pixbuf_rotate_simple(pixbuf,270);
        gtk_image_set_from_pixbuf(GTK_IMAGE(image), pixbuf);
    }
}

static void RightRotate_pixbuf () {
    if (pixbuf != NULL) {
        pixbuf = gdk_pixbuf_rotate_simple(pixbuf,90);
        gtk_image_set_from_pixbuf(GTK_IMAGE(image), pixbuf);
    }
}
//-----------------------------------------------------------//


static void choose_file (GtkWidget *sender, gpointer data)
{
    g_print("[OCR] - Choose picture\n");

    GtkWidget *dialog;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
    gint res;
    char* filename;
    int height, width;

    filename = "";

    dialog = gtk_file_chooser_dialog_new ("Open File",
                                          (GtkWindow *) data,
                                          action,
                                          ("_Cancel"),
                                          GTK_RESPONSE_CANCEL,
                                          ("_Open"),
                                          GTK_RESPONSE_ACCEPT,
                                          NULL);

    res = gtk_dialog_run (GTK_DIALOG (dialog));
    if (res == GTK_RESPONSE_ACCEPT)
    {
        GtkFileChooser *chooser = GTK_FILE_CHOOSER (dialog);
        filename = gtk_file_chooser_get_filename (chooser);
        opendir (filename);
    }
    gtk_widget_destroy (dialog);

    gtk_label_set_text(GTK_LABEL(imagePath), filename);

    pixbuf = gdk_pixbuf_new_from_file (filename, NULL);
        // Auto resize image
    if (gdk_pixbuf_get_width(pixbuf) > gdk_pixbuf_get_height(pixbuf)) {
        width = 800;
        height = width*gdk_pixbuf_get_height(pixbuf)/gdk_pixbuf_get_width(pixbuf) ;
    } else {
        height = 800;
        width = height*gdk_pixbuf_get_width(pixbuf)/gdk_pixbuf_get_height(pixbuf) ;
    }
    pixbuf = gdk_pixbuf_scale_simple(pixbuf, width, height, GDK_INTERP_BILINEAR);

    gtk_image_set_from_pixbuf(GTK_IMAGE(image), pixbuf);

    g_print("[OCR] - Load picture : %s\n", filename);
}



        // Main function
int gui_main(int argc, char* argv[])
{

    g_print("[OCR] - Application launched\n");


    // Var Defines
        // Widget
    GtkWidget *window, *grid;
        // Buttons
    GtkWidget *Selectbutton, *RotLeftButton, *RotRightButton, *PreTreatementButton;
    GtkWidget *GrayScaleButton, *ContrastButton, *CharacterFinderButton;
    //GtkWidget *TrainingCheck;
        // Labels
    GtkWidget *title;


    // Init window
    gtk_init(&argc, &argv);
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    g_signal_connect(window, "destroy", G_CALLBACK(destroy), NULL);
    gtk_container_set_border_width(GTK_CONTAINER(window), 100);


    // Init Grid (modules placements)
    grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), grid); // attach grid to window


    // Training Checkbox
    TrainingCheck = gtk_check_button_new_with_label ("Entrainement");
    gtk_grid_attach(GTK_GRID(grid), TrainingCheck, 6, 6, 1, 1);
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (TrainingCheck), FALSE);


    // Select Button
    Selectbutton = gtk_button_new_with_label("Select file");
    g_signal_connect(Selectbutton,
                     "clicked", G_CALLBACK(choose_file),
                     window);
        
        // attach to grid   
    gtk_grid_attach(GTK_GRID(grid), Selectbutton, 2, 1, 1, 1);


//--------------------ROTATIONS-BUTTONS----------------------//
    RotLeftButton = gtk_button_new_with_label("<<");
    RotRightButton = gtk_button_new_with_label(">>");
    g_signal_connect(RotLeftButton,
                     "clicked",
                     LeftRotate_pixbuf,
                     "button");    
    g_signal_connect(RotRightButton,
                    "clicked",
                    RightRotate_pixbuf,
                    "button");
    gtk_grid_attach(GTK_GRID(grid), RotLeftButton, 0, 8, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), RotRightButton, 4, 8, 1, 1);

    // Run pre-work image
    PreTreatementButton = gtk_button_new_with_label("Run pre-treatement");
    g_signal_connect(PreTreatementButton,
                    "clicked",
                    RunImageProcessing,
                    "button");
    gtk_grid_attach(GTK_GRID(grid), PreTreatementButton, 1, 9, 3, 1);


//----------------------PRE-TREATEMENTS-BUTTONS--------------//
    // GrayScaleButton
    GrayScaleButton = gtk_button_new_with_label("Run Gray Scale");
    g_signal_connect(GrayScaleButton,
                    "clicked",
                    GrayScaleProcess,
                    NULL );
    gtk_grid_attach(GTK_GRID(grid), GrayScaleButton, 6, 3, 1, 1);

    // ContrastButton
    ContrastButton = gtk_button_new_with_label("Run Contrast");
    g_signal_connect(ContrastButton,
                    "clicked",
                    ContrastProcess,
                    NULL );
    gtk_grid_attach(GTK_GRID(grid), ContrastButton, 6, 4, 1, 1);

    // CharacterFinder
    CharacterFinderButton = gtk_button_new_with_label("Run Character Finder");
    g_signal_connect(CharacterFinderButton,
                    "clicked",
                    CharacterFinderProcess,
                    NULL );
    gtk_grid_attach(GTK_GRID(grid), CharacterFinderButton, 6, 5, 1, 1);

//-----------------------------------------------------------//













    // Selected Image
    imagePath = gtk_label_new ("No picture imported");
    gtk_grid_attach(GTK_GRID(grid), imagePath, 2, 2, 1, 1);
    pixbuf = gdk_pixbuf_new_from_file ("", NULL);
    image = gtk_image_new_from_file ("");
    gtk_grid_attach(GTK_GRID(grid), image, 0, 3, 5, 5);

    // Labels
    title = gtk_label_new ("Optical Character Recognition\n");
    gtk_grid_attach(GTK_GRID(grid), title, 0, 0, 5, 1);



        // Draw window - END MAIN FUNCTION
    gtk_widget_show_all(window);
    gtk_main();
    return 0;
}