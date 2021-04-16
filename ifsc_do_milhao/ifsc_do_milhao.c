#include <allegro5/allegro.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include "biblioteca.h"

//GLOBALS==============================
const int WIDTH = 800;
const int HEIGHT = 800;

//prototypes
void Boot(ALLEGRO_BITMAP *bootimage); //iniciação do jogo
void Thematic(); //modalidade das perguntas
int CheckMouse(ALLEGRO_EVENT_QUEUE *event_queue, ALLEGRO_BITMAP *op1, ALLEGRO_BITMAP *op2, ALLEGRO_BITMAP *op3); //checa a posição do mouse sobre as opções na tela e o clique sobre o botão de sair

void GameInitiation(int theme, ALLEGRO_BITMAP *exit); //iniciação da rodada
int NewQuestion(int theme); //gera uma pergunta nova
int Points(struct Character *player); //Pontuação do jogador, quantos pedidos de ajuda ainda podem ser solicitados, etc
bool Answer(int answer, int realanswer, struct Character *player); //analise da resposta

void ChooseCharacter (struct Character *player); //escolha do personagem do jogador
void Character(struct Character *player, int charc); //inicia o personagem do jogador
void CharacterUpdate(struct Character *player, int charc); //atualiza o personagem do jogador
void Professor(struct Extras *x, struct Extras *y); //aparição de um professor
void ProfessorUpdate(struct Extras *x, struct Extras *y); //atualiza o professor (quando há pedido de ajuda para os universitários)
int Interviewer(struct Extras *pamela, struct Extras *valter, int quest); //aparição de um entrevistador
void InterviewerUpdate(int interviewer); //atualiza o personagem do entrevistador

void Help(struct Extras *x, struct Extras *y); //ajuda aos universitários
void Stop(ALLEGRO_EVENT_QUEUE *event_queue, ALLEGRO_BITMAP *exit); //parar de jogar
void Tutorial(); //explicação do funcionamento do jogo

void Error(char *text){
	al_show_native_message_box(NULL,"ERRO", "Ocorreu o seguinte erro e o programa sera finalizado:",text,NULL,ALLEGRO_MESSAGEBOX_ERROR);
}

int main(void) {
	//primitive variable
	bool done = false; //exit
	bool redraw = true;
	const int FPS = 60;
	bool isGameOver = false;
	int thematic; //temica escolhida
	int playr; //jogador escolhido
	int question; //questao da vez
	int interv; //entrevistador da vez
	int answer; //resposta dada pelo usuario
	int feedback; //gabarito da questao

	//object variables
	struct Character player;
	struct Extras pamela;
	struct Extras valter;
	struct Extras x; //professor 1
	struct Extras y; //professor 2 etc

	//Allegro variables
	ALLEGRO_DISPLAY *display = NULL;
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	ALLEGRO_TIMER *timer = NULL;
	ALLEGRO_FONT *font18 = NULL;
	ALLEGRO_BITMAP *exit = NULL, *option1 = NULL, *option2 = NULL, *option3 = NULL;
	ALLEGRO_BITMAP *bootimage = NULL;

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

	//carrega o fundo
    bootimage = al_load_bitmap("/Users/pamela_fialho/Documents/GitHub/ifsc_do_milhao/ifsc_do_milhao/boot.png");
    if (!bootimage){
        error_msg("Falha ao carregar fundo");
        al_destroy_display(display);
        return 0;
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

	Boot(); //inicia o jogador
	ChooseCharacter (&player); //mostra as opções de personagem
	playr = CheckMouse(event_queue, op1, op2, op3); //checa a opção escolhida pelo usuário
	Thematic(); //mostra as opções de temática para as perguntas
	thematic = CheckMouse(event_queue, op1, op2, op3); //checa a opção escolhida pelo usuário
	Character(&player, playr); //inicia o personagem do jogador
	Interviewer(&pamela, &valter); //inicia o personagem do entrevistador
	GameInitiation(thematic, exit); //inicia o jogo
	CharacterUpdate(&player, playr);
	question = NewQuestion(thematic);
	interv = Interviewer(&pamela, &valter, question);
	InterviewerUpdate(interv);

	font18 = al_load_font("/Users/pamela_fialho/Documents/GitHub/ifsc_do_milhao/ifsc_do_milhao/arial.ttf", 18, 0);

	al_register_event_source(event_queue, al_get_mouse_event_source());
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	al_register_event_source(event_queue, al_get_display_event_source(display));
	
	while(!done) {
		ALLEGRO_EVENT ev;
		al_wait_for_event(event_queue, &ev);

		if(ev.type == ALLEGRO_EVENT_TIMER) {
			//teclado tutorial "T"
			//sair do jogo
			redraw = true;
			if(!isGameOver) {
				NewQuestion(thematic);
				answer = CheckMouse(event_queue, op1, op2, op3); //checa a opção escolhida pelo usuário
				Answer(answer, feedback, &player);
				
			}
		}
		else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			done = true;
		}
		if(redraw && al_is_event_queue_empty(event_queue)) {
			redraw = false; 
			if(!isGameOver) {
				al_draw_textf(font18, al_map_rgb(255, 0, 255), 5, 5, 0, "Player has %i lives left. Player has destroyed %i objects", player.lives, player.score);
				if (Points(&player) == 1) {
					al_draw_textf(font18, al_map_rgb(0, 255, 255), WIDTH / 2, HEIGHT / 2, ALLEGRO_ALIGN_CENTRE, "Parabéns! Você se formou com 10", player.score); //mensagem para o ganhador //arrumar para variar conforme o personagem do jogador
				}
			}
			else {
				al_draw_textf(font18, al_map_rgb(0, 255, 255), WIDTH / 2, HEIGHT / 2, ALLEGRO_ALIGN_CENTRE, "Game Over. Final Score: %i", player.score);
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

void Boot(ALLEGRO_BITMAP *bootimage) {
	//desenha o fundo na tela
    al_draw_bitmap_region(bootimage,0,0,WIDTH,HEIGHT,0,0,0);
}

void Thematic() {
    //colocar fundo do display com imagem para esta escolha
	//colocar textos em cima dos blocos de opções
}

void GameInitiation(int theme, ALLEGRO_BITMAP *exit) {
	al_set_target_bitmap(exit);
    al_clear_to_color(al_map_rgb(255, 0, 0)); //pinta botão de sair do jogo de vermelho
	//botao de ajuda para universitarios
}

void ChooseCharacter (struct Character *player) {
	//pergunta qual jogador ele quer ser "professor", "aluno" ou "aleatorio"
}

void Character(struct Character *player, int charc) { //inicia o personagem
	player->ID = charc;
	player->x = 25;
	player->y = HEIGHT;
	player->lives = 3;
	player->score = 0;
}

void CharacterUpdate(struct Character *player, int charc); {
	for (int i=player->y; i>HEIGHT-25; i--) {
		player->y = i;
	}
}

void Professor(struct Extras *x, struct Extras *y) {
	char choice;
	//pergunta qual jogador ele quer ser "professor", "aluno" ou "aleatorio"
	//ler a escolha
	if () {

	}
	x->ID = choice;
	x->x = 25;
	x->y = HEIGHT;
}

void ProfessorUpdate(struct Extras *x, struct Extras *y) {

}

int Interviewer(struct Extras *pamela, struct Extras *valter, int quest) {
	int interv; //qual entrevistador da vez

	return interv;
}

void InterviewerUpdate(int interviewer) {

}

int NewQuestion(int theme) { //implementar logica para nao repetir pergunta
	int quest;
	if (theme == 1) {
		//print de uma pergunta aleatoria dentre todas as categorias
	}
	if (theme == 2) {
		//print de uma pergunta aleatoria dentre as de professor
	}
	if (theme == 3) {
		//print de uma pergunta aleatoria dentre as de aluno
	}
	return quest;
}

int CheckMouse(ALLEGRO_EVENT_QUEUE *event_queue, ALLEGRO_BITMAP *op1, ALLEGRO_BITMAP *op2, ALLEGRO_BITMAP *op3) { //melhorar usando vetores
	int result;
	while (!al_is_event_queue_empty(event_queue)){
        ALLEGRO_EVENT ev;
        al_wait_for_event(event_queue, &ev);
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

		//se o evento foi um clique do mouse
		if (ev.mouse.x >= al_get_bitmap_width(op1) - 150 &&
        ev.mouse.x <= al_get_bitmap_width(op1) + 150 &&
    	ev.mouse.y >= al_get_bitmap_height(op1) - 50 &&
    	ev.mouse.y <= al_get_bitmap_height(op1) + 50){
        	result = 1;
        }
		else if (ev.mouse.x >= al_get_bitmap_width(op2) - 150 &&
        ev.mouse.x <= al_get_bitmap_width(op2) + 150 &&
    	ev.mouse.y >= al_get_bitmap_height(op2) - 50 &&
    	ev.mouse.y <= al_get_bitmap_height(op2) + 50){
        	result = 2;
        }
		else if (ev.mouse.x >= al_get_bitmap_width(op3) - 150 &&
        ev.mouse.x <= al_get_bitmap_width(op3) + 150 &&
    	ev.mouse.y >= al_get_bitmap_height(op3) - 50 &&
    	ev.mouse.y <= al_get_bitmap_height(op3) + 50){
        	result = 3;
        }
	}
return result;
}

bool Answer(int answer, int realanswer, struct Character *player) {
	bool point;
	if (answer == realanswer) {
		point = true;
		player->score++;
	}
	return point;
}

void Stop(ALLEGRO_EVENT_QUEUE *event_queue, ALLEGRO_BITMAP *exit) {
	// Verificamos se há eventos na fila
    while (!al_is_event_queue_empty(event_queue)){
        ALLEGRO_EVENT ev;
    	al_wait_for_event(event_queue, &ev);
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

int Points(struct Character *player) {
	if (player->score == 10) {
		return 1;
	}
}