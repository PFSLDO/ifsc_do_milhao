#include <allegro5/allegro.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include "biblioteca.h"

//GLOBALS==============================
const int WIDTH = 800;
const int HEIGHT = 800;

//prototypes
void Boot(); //iniciação do jogo
void Thematic(event_queue, option1, option2, option3); //modalidade das perguntas
void GameInitiation(int theme); //iniciação da rodada
void NewQuestion(); //gera uma pergunta nova
void Points(); //Pontuação do jogador, quantos pedidos de ajuda ainda podem ser solicitados, etc
void Answer(int ans); //analise da resposta

void Character(); //escolha  de personagem
void CharacterUpdate(); //atualiza o personagem do jogador
void Professor(); //aparição de um professor
void ProfessorUpdate(); //atualiza o professor (quando há pedido de ajuda para os universitários)
void Interviewer(); //aparição de um entrevistador
void InterviewerUpdate(); //atualiza o personagem do entrevistador

void Help(); //ajuda aos universitários
void Stop(); //parar de jogar
void Tutorial(); //explicação do funcionamento do jogo

// void InitShip(struct SpaceShip *ship);
// void DrawShip(struct SpaceShip *ship);
// void MoveShipUp(struct SpaceShip *ship);
// void MoveShipDown(struct SpaceShip *ship);
// void MoveShipLeft(struct SpaceShip *ship);
// void MoveShipRight(struct SpaceShip *ship);

// void InitBullet(struct Bullet bullet[], int size);
// void DrawBullet(struct Bullet bullet[], int size);
// void FireBullet(struct Bullet bullet[], int size, struct SpaceShip *ship);
// void UpdateBullet(struct Bullet bullet[], int size);
// void CollideBullet(struct Bullet bullet[], int bSize, struct Comet comets[], int cSize, struct SpaceShip *ship);

// void InitComet(struct Comet comets[], int size);
// void DrawComet(struct Comet comets[], int size);
// void StartComet(struct Comet comets[], int size);
// void UpdateComet(struct Comet comets[], int size);
// void CollideComet(struct Comet comets[], int cSize, struct SpaceShip *ship);

void Error(char *text){
	al_show_native_message_box(NULL,"ERRO", "Ocorreu o seguinte erro e o programa sera finalizado:",text,NULL,ALLEGRO_MESSAGEBOX_ERROR);
}

int main(void) {
	//primitive variable
	bool done = false; //exit
	bool redraw = true;
	const int FPS = 60;
	bool isGameOver = false;

	//object variables
	// struct SpaceShip ship;
	// struct Bullet bullets[NUM_BULLETS];
	// struct Comet comets[NUM_COMETS];

	//Allegro variables
	ALLEGRO_DISPLAY *display = NULL;
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	ALLEGRO_TIMER *timer = NULL;
	ALLEGRO_FONT *font18 = NULL;
	ALLEGRO_BITMAP *exit = NULL, *option1 = NULL, *option2 = NULL, *option3 = NULL;

	al_init_primitives_addon();
	al_install_mouse();
	al_init_font_addon();
	al_init_ttf_addon();

	//Initialization Functions
	if (!al_init()){ 
        Error("Falha ao inicializar o Allegro");
        return -1;
    }

	display = al_create_display(WIDTH, HEIGHT);			//create our display object
	if(!display) {
		Error("Falha ao criar o display");									//test display object
		return -1;
	}

	// Configura o título da janela
    al_set_window_title(display, "IFSC do Milhão");

	// Torna apto o uso de mouse na aplicação
    if (!al_install_mouse()){
        Error("Falha ao inicializar o mouse");
        al_destroy_display(display);
        return -1;
    }

	// Atribui o cursor padrão do sistema para ser usado
    if (!al_set_system_mouse_cursor(display, ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT)){
        Error("Falha ao atribuir ponteiro do mouse");
        al_destroy_display(display);
        return -1;
    }

	// Alocamos a primeira opção de resposta/escolha
    option1 = al_create_bitmap(WIDTH/2, HEIGHT/2+25);
    if (!option1){
        Error("Falha ao criar bitmap");
        al_destroy_display(display);
        return -1;
    }

	// Alocamos a segunda opção de resposta/escolha
    option2 = al_create_bitmap(WIDTH/2, HEIGHT/2+150);
    if (!option2){
        Error("Falha ao criar bitmap");
        al_destroy_display(display);
        return -1;
    }

	// Alocamos a terceira opção de resposta/escolha
    option3 = al_create_bitmap(WIDTH/2, HEIGHT/2+275);
    if (!option3){
        Error("Falha ao criar bitmap");
        al_destroy_display(display);
        return -1;
    }

	// Alocamos o botão para fechar a aplicação
    exit = al_create_bitmap(WIDTH-100, 100);
    if (!exit){
        Error("Falha ao criar botão de saída");
        al_destroy_bitmap(exit);
        al_destroy_display(display);
        return -1;
    }

	event_queue = al_create_event_queue();
    if (!event_queue){
        Error("Falha ao inicializar o fila de eventos");
        al_destroy_display(display);
        return -1;
    }

	timer = al_create_timer(1.0 / FPS);
	if (!timer){
        Error("Falha ao inicializar o temporizador");
        al_destroy_display(display);
        return -1;
    }

	// Dizemos que vamos tratar os eventos vindos do mouse
    al_register_event_source(event_queue, al_get_mouse_event_source());

	srand(time(NULL));
	// InitShip(&ship);
	// InitBullet(bullets, NUM_BULLETS);
	// InitComet(comets, NUM_COMETS);
	
	font18 = al_load_font("/Users/pamela_fialho/Documents/GitHub/ifsc_do_milhao/ifsc_do_milhao/arial.ttf", 18, 0);

	al_register_event_source(event_queue, al_get_mouse_event_source());
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	al_register_event_source(event_queue, al_get_display_event_source(display));
	
	while(!done) {
		ALLEGRO_EVENT ev;
		al_wait_for_event(event_queue, &ev);

		if(ev.type == ALLEGRO_EVENT_TIMER) {
			// redraw = true;
			// if(keys[UP])
			// 	MoveShipUp(&ship);
			// if(keys[DOWN])
			// 	MoveShipDown(&ship);
			// if(keys[LEFT])
			// 	MoveShipLeft(&ship);
			// if(keys[RIGHT])
			// 	MoveShipRight(&ship);

			if(!isGameOver) {
				// UpdateBullet(bullets, NUM_BULLETS);
				// StartComet(comets, NUM_COMETS);
				// UpdateComet(comets, NUM_COMETS);
				// CollideBullet(bullets, NUM_BULLETS, comets, NUM_COMETS, &ship);
				// CollideComet(comets, NUM_COMETS, &ship);

				// if(ship.lives <= 0) {
				// 	isGameOver = true;
				// }
			}
		}
		else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			done = true;
		}
		if(redraw && al_is_event_queue_empty(event_queue)) {
			redraw = false; 

			if(!isGameOver) {
				// DrawShip(&ship);
				// DrawBullet(bullets, NUM_BULLETS);
				// DrawComet(comets, NUM_COMETS);

				// al_draw_textf(font18, al_map_rgb(255, 0, 255), 5, 5, 0, "Player has %i lives left. Player has destroyed %i objects", ship.lives, ship.score);
			}
			else {
				// al_draw_textf(font18, al_map_rgb(0, 255, 255), WIDTH / 2, HEIGHT / 2, ALLEGRO_ALIGN_CENTRE, "Game Over. Final Score: %i", ship.score);
			}
		
			al_flip_display();
			al_clear_to_color(al_map_rgb(0,0,0));
		}
	}
	al_destroy_bitmap(exit);
    al_destroy_bitmap(option1);
	al_destroy_bitmap(option2);
	al_destroy_bitmap(option3);
	al_destroy_event_queue(event_queue);
	al_destroy_timer(timer);
	al_destroy_font(font18);
	al_destroy_display(display);						//destroy our display object

	return 0;
}

void Thematic(ev_queue, op1, op2, op3) { //melhorar usando vetores
	bool general = false, professor = false, student = false;

    // Verificamos se há eventos na fila
    while (!al_is_event_queue_empty(ev_queue)){
        ALLEGRO_EVENT ev;
        al_wait_for_event(ev_queue, &ev);
 
        // Se o evento foi de movimentação do mouse
        if (ev.type == ALLEGRO_EVENT_MOUSE_AXES){
			//Verifica se o cursor está sobre a opção 1
			if (ev.mouse.x >= al_get_bitmap_width(op1) - 150 &&
            ev.mouse.x <= al_get_bitmap_width(op1) + 150 &&
        	ev.mouse.y >= al_get_bitmap_height(op1) - 50 &&
        	ev.mouse.y <= al_get_bitmap_height(op1) + 50){
            	// Colorimos o bitmap correspondente ao retângulo que o cursor está por cima
        		al_set_target_bitmap(op1);
				al_clear_to_color(al_map_rgb(0, 255, 0));
        	}
			else {
				al_clear_to_color(al_map_rgb(255, 255, 255));
			}

			//Verifica se o cursor está sobre a opção 2
			if (ev.mouse.x >= al_get_bitmap_width(op2) - 150 &&
            ev.mouse.x <= al_get_bitmap_width(op2) + 150 &&
        	ev.mouse.y >= al_get_bitmap_height(op2) - 50 &&
        	ev.mouse.y <= al_get_bitmap_height(op2) + 50){
            	// Colorimos o bitmap correspondente ao retângulo que o cursor está por cima
        		al_set_target_bitmap(op2);
				al_clear_to_color(al_map_rgb(0, 255, 0));
        	}
			else {
				al_clear_to_color(al_map_rgb(255, 255, 255));
			}

			//Verifica se o cursor está sobre a opção 3
			if (ev.mouse.x >= al_get_bitmap_width(op3) - 150 &&
            ev.mouse.x <= al_get_bitmap_width(op3) + 150 &&
        	ev.mouse.y >= al_get_bitmap_height(op3) - 50 &&
        	ev.mouse.y <= al_get_bitmap_height(op3) + 50){
            	// Colorimos o bitmap correspondente ao retângulo que o cursor está por cima
        		al_set_target_bitmap(op3);
				al_clear_to_color(al_map_rgb(0, 255, 0));
        	}
			else {
				al_clear_to_color(al_map_rgb(255, 255, 255));
			}
        }

        //se o evento foi um clique do mouse
		else if (ev.mouse.x >= al_get_bitmap_width(op1) - 150 &&
        ev.mouse.x <= al_get_bitmap_width(op1) + 150 &&
    	ev.mouse.y >= al_get_bitmap_height(op1) - 50 &&
    	ev.mouse.y <= al_get_bitmap_height(op1) + 50){
        	GameInitiation(1); //chama a função que analisa a resposta que foi pressionada
        }
		else if (ev.mouse.x >= al_get_bitmap_width(op2) - 150 &&
        ev.mouse.x <= al_get_bitmap_width(op2) + 150 &&
    	ev.mouse.y >= al_get_bitmap_height(op2) - 50 &&
    	ev.mouse.y <= al_get_bitmap_height(op2) + 50){
        	GameInitiation(2); //chama a função que analisa a resposta que foi pressionada
        }
		else if (ev.mouse.x >= al_get_bitmap_width(op3) - 150 &&
        ev.mouse.x <= al_get_bitmap_width(op3) + 150 &&
    	ev.mouse.y >= al_get_bitmap_height(op3) - 50 &&
    	ev.mouse.y <= al_get_bitmap_height(op3) + 50){
        	GameInitiation(3); //chama a função que analisa a resposta que foi pressionada
        }
	}
}

void GameInitiation(int theme, exit) {
	al_set_target_bitmap(exit);
    al_clear_to_color(al_map_rgb(255, 0, 0));
	al_draw_bitmap(exit, LARGURA_TELA - al_get_bitmap_width(exit) - 10,
        ALTURA_TELA - al_get_bitmap_height(exit) - 10, 0);
	
	for (int i=0; i<3; i++) {
		if (theme == i) {
			NewQuestion(i);
		}
	}
}

void Answer(int answer) {
	if (answer == ) {
		
	}
}

void Stop(ev_queue, exit) {
	// Verificamos se há eventos na fila
    while (!al_is_event_queue_empty(ev_queue)){
        ALLEGRO_EVENT ev;
    	al_wait_for_event(ev_queue, &ev);
		if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP){
            if (ev.mouse.x >= al_get_bitmap_width(exit) - 20 &&
            ev.mouse.x <= al_get_bitmap_width(exit) + 20 &&
			ev.mouse.y <= al_get_bitmap_height(exit) + 10 &&
            ev.mouse.y >= al_get_bitmap_height(exit) - 10){
				return 1;
            }
    	}
	}
}