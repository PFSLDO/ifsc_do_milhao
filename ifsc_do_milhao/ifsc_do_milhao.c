#include <allegro5/allegro.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include "biblioteca.h"

//GLOBALS==============================
const int WIDTH = 800;
const int HEIGHT = 800;
enum KEYS {SPACE, ESC, NUM1, NUM2, NUM3, P, A};
enum STATE {MENU, CHOOSE_CHARACTER, CHOOSE_THEMATIC, PLAYING, GAMEOVER, WON};

//prototypes
void ChooseThematic(int which); //modalidade das perguntas
int CheckMouse(ALLEGRO_EVENT_QUEUE *event_queue, ALLEGRO_BITMAP *op1, ALLEGRO_BITMAP *op2, ALLEGRO_BITMAP *op3); //checa a posição do mouse sobre as opções na tela e o clique sobre o botão de sair

void GameInitiation(int theme); //iniciação da rodada
int NewQuestion(int theme); //gera uma pergunta nova
bool Answer(int answer, int realanswer, struct Character *player); //analise da resposta

void ChooseCharacter (struct Character *player, int which); //escolha do personagem do jogador
void Character(struct Character *player, int charc); //inicia o personagem do jogador
void CharacterUpdate(struct Character *player, int charc); //atualiza o personagem do jogador
void Professor(struct Extras *x, struct Extras *y); //aparição de um professor
void ProfessorUpdate(struct Extras *x, struct Extras *y); //atualiza o professor (quando há pedido de ajuda para os universitários)
int Interviewer(struct Extras *pamela, struct Extras *valter, int quest); //aparição de um entrevistador
void InterviewerUpdate(int interviewer); //atualiza o personagem do entrevistador
void Help(struct Extras *x, struct Extras *y); //ajuda aos universitários

void Error(char *text){
	al_show_native_message_box(NULL,"ERRO", "Ocorreu o seguinte erro e o programa sera finalizado:",text,NULL,ALLEGRO_MESSAGEBOX_ERROR);
}

int main(void) {
	//primitive variable
	const int FPS = 60;
	bool done = false;
	bool redraw = true;
	bool isGameOver = false;
	bool FirstTime = true;
	bool keys[7] = {false, false, false, false, false, false, false};
	int state = MENU; //inicia no menu

	int thematic; //temica escolhida
	int playr; //jogador escolhido
	int question; //questao da vez
	int interv; //entrevistador da vez
	int answer; //resposta dada pelo usuario
	int feedback; //gabarito da questao

	//object variables
	struct Character player; //cria o jogador
	struct Extras pamela; //cria a entrevistadora Pamela
	struct Extras valter; //cria o entrevistador Valter
	struct Extras x; //cria o professor x
	struct Extras y; //cria o professor y

	//Allegro variables
	ALLEGRO_DISPLAY *display = NULL;
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	ALLEGRO_TIMER *timer = NULL;
	ALLEGRO_FONT *font = NULL;
	ALLEGRO_BITMAP *option1 = NULL;
	ALLEGRO_BITMAP *option2 = NULL;
	ALLEGRO_BITMAP *option3 = NULL;

	//Allegro image variables
	ALLEGRO_BITMAP *menuimage = NULL;

	//Allegro audio variables
	ALLEGRO_SAMPLE *game_theme = NULL;
    ALLEGRO_SAMPLE_INSTANCE *game_theme_instance = NULL;

	//Initialization Functions
	if (!al_init()){ 
        Error("Falha ao inicializar o Allegro");
        return -1;
    }

	if(!al_install_audio()) {
        return -1;
        printf("Erro ao iniciar os sistemas de Audio!\n");
    }

    if(!al_init_acodec_addon()) {
        return -1;
        printf("Erro ao iniciar os sistemas de codec de Audio!\n");
    }

    if (!al_reserve_samples(1)) {
        return -1;
        printf("Erro ao reservar as samples de Audio!\n");
    }

	display = al_create_display(WIDTH, HEIGHT);			//create our display object
	
	if(!display) {
		Error("Falha ao criar o display");									//test display object
		return -1;
	}

	if(!al_init_primitives_addon()) {
        return -1;
        printf("Erro inciar os addons primitivos!\n");
    }

    if(!al_install_keyboard()) {
        return -1;
        printf("Erro instalar/iniciar o teclado!\n");
    }

    if(!al_init_image_addon()) {
        return -1;
        printf("Erro iniciar o addon de imagem!\n");
    }

	al_init_primitives_addon();
	al_install_mouse();
	al_init_font_addon();
	al_init_ttf_addon();
	al_init_image_addon();

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

	//Carrega os arquivos utilizados
    menuimage = al_load_bitmap("/Users/pamela_fialho/Documents/GitHub/ifsc_do_milhao/ifsc_do_milhao/menu_image.png"); //carrega a imagem do menu
    option1 = al_create_bitmap(WIDTH/2, HEIGHT/2+25);	// Alocamos a primeira opção de resposta/escolha
    option2 = al_create_bitmap(WIDTH/2, HEIGHT/2+150);
    option3 = al_create_bitmap(WIDTH/2, HEIGHT/2+275);
	font = al_load_font("/Users/pamela_fialho/Documents/GitHub/Listas_de_Exercicios_Programacao_em_Linguagem_C/atividade_expansao_dos_cometas/arial.ttf", 18, 0);
	//Carrega o Audio
	al_reserve_samples(2);//Reserva 2 samples, mais do que o suficiente para o que vai ser usado
    game_theme = al_load_sample("Audio/pacman_theme.wav");//carrega o tema do pacman
    game_theme_instance = al_create_sample_instance(game_theme);//cria uma sample instance e coloca o tema dentro
    al_set_sample_instance_playmode(game_theme_instance, ALLEGRO_PLAYMODE_LOOP);//configura o playmode da sample instance, nesse caso loop
    al_attach_sample_instance_to_mixer(game_theme_instance, al_get_default_mixer());//da o "atach" da sample_instance ao mixer

	event_queue = al_create_event_queue();
	timer = al_create_timer(1.0 / FPS);

	al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_timer_event_source(timer));

	al_start_timer(timer);//Inicia o timer
    al_play_sample_instance(game_theme_instance);//Começa a tocar o tema
	
	while(!done) {
		ALLEGRO_EVENT ev;
		al_wait_for_event(event_queue, &ev);
		if(ev.type == ALLEGRO_EVENT_KEY_DOWN) {
			switch(ev.keyboard.keycode) {
				case ALLEGRO_KEY_ESCAPE:
                	keys[ESC] = true;
                	break;
				case ALLEGRO_KEY_SPACE:
                	keys[SPACE] = true;
                	break;
				case ALLEGRO_KEY_P:
                	keys[P] = true;
                	break;
				case ALLEGRO_KEY_A:
                	keys[A] = true;
                	break;
				case ALLEGRO_KEY_1:
                	keys[NUM1] = true;
                	break;
				case ALLEGRO_KEY_2:
                	keys[NUM2] = true;
                	break;
				case ALLEGRO_KEY_3:
                	keys[NUM3] = true;
                	break;
			}
		}
		else if(ev.type == ALLEGRO_EVENT_KEY_UP) {
            switch(ev.keyboard.keycode) {
            case ALLEGRO_KEY_SPACE:
                keys[SPACE] = false;
                break;
            case ALLEGRO_KEY_ESCAPE:
                keys[ESC] = false;
                break;
			case ALLEGRO_KEY_P:
                keys[P] = false;
            	break;
			case ALLEGRO_KEY_A:
            	keys[A] = false;
            	break;
			case ALLEGRO_KEY_1:
                keys[NUM1] = false;
            	break;
			case ALLEGRO_KEY_2:
            	keys[NUM2] = false;
            	break;
			case ALLEGRO_KEY_3:
            	keys[NUM3] = false;
            	break;
            }
        }
		else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            done = true;
        }
		else if(ev.type == ALLEGRO_EVENT_TIMER) {
			redraw = true;
			if (state == MENU) {
                if(keys[SPACE])
                    state = CHOOSE_CHARACTER;
                if(keys[ESC])
                    done = true;
            }
			else if (state == CHOOSE_CHARACTER) {
                if(keys[P])
					playr = 2;
					ChooseCharacter(&player, playr);
					state = CHOOSE_THEMATIC;

				if(keys[A])
					playr = 1;
					ChooseCharacter(&player, playr);
					state = CHOOSE_THEMATIC;
                if(keys[ESC])
                    done = true;
            }
			else if (state == CHOOSE_THEMATIC) {
				for(int i=1; i<4; i++) {
					if(keys[i])
						thematic = i;
						ChooseThematic(thematic);
						state = PLAYING;
				}
                if(keys[ESC])
                    done = true;
            }
			else if (state == PLAYING) {
                if(FirstTime) { //Roda apenas ao entrar no mapa pela primeira vez
                    Character(&player, playr); //inicia o personagem do jogador
					CharacterUpdate(&player, playr); //atualiza o personagem do jogador
					question = NewQuestion(thematic);
					interv = Interviewer(&pamela, &valter, question); //aparição de um entrevistador
					InterviewerUpdate(interv); //atualiza o personagem do entrevistador
                    FirstTime = false; //Joga firstTime para false, de modo a não entrar no If novamente
                }
                if(keys[ESC])
                    state = GAMEOVER;
            }
			else if (state == GAMEOVER)
            {
                if(keys[ESC])
                    done = true;
                else if (keys[SPACE]) {
                    state = MENU;
                }
            }

			if(redraw && al_is_event_queue_empty(event_queue)) {
            	redraw = false;
				if (state == MENU) {
					al_draw_bitmap(menuimage,0,0,0);
					//textos
					//tutorial
					al_draw_text(font, al_map_rgb(0,0,255), WIDTH/2, 200, ALLEGRO_ALIGN_CENTER, "COMO JOGAR:");
                	al_draw_text(font, al_map_rgb(255,40,40), 240, 220, ALLEGRO_ALIGN_CENTER, "PRESSIONE ESC A QUALQUER MOMENTO");
                	al_draw_text(font, al_map_rgb(255,40,40), 240, 245, ALLEGRO_ALIGN_CENTER, "PARA SAIR DO JOGO");
                	al_draw_text(font, al_map_rgb(255,255,0), 240, 280, ALLEGRO_ALIGN_CENTER, "APERTE SPACE PARA SER");
                	al_draw_text(font, al_map_rgb(255,255,0), 240, 305, ALLEGRO_ALIGN_CENTER, "REDIRECIONADO A PARTIDA");
                	al_draw_text(font, al_map_rgb(0,255,0), 240, 340, ALLEGRO_ALIGN_CENTER, "BOA SORTE!");
                	al_draw_text(font, al_map_rgb(0,0,255), 280, 400, ALLEGRO_ALIGN_CENTER, "DESENVOLVIDO POR:");
                	al_draw_text(font, al_map_rgb(255,255,255), 40, 460, 0, "Pamela Fialho");
                	al_draw_text(font, al_map_rgb(255,255,255), 40, 490, 0, "Valter da Silva");
                	al_draw_text(font, al_map_rgb(0,0,255), 250, 550, ALLEGRO_ALIGN_CENTER, "Sob orientação de:");
                	al_draw_text(font, al_map_rgb(255,255,255), 40, 610, 0, "Professor Fernando Santana Pacheco");
                	al_draw_text(font, al_map_rgb(255,255,255), 40, 640, 0, "Mike Geig (FixByProximity)");
                	al_draw_text(font, al_map_rgb(255,255,255), 40, 670, 0, "TURMA 722 (2017/2)");
            	}
				if (state == CHOOSE_CHARACTER) {
					//imagem menu de personagem
					//escolha com o teclado (A - aluno; P - professor)
				}
				if (state == CHOOSE_THEMATIC) {
					//imagem menu de tematica
					//escolha com o teclado (1 - x; 2 - y; 3 - todas)
				}
            	else if (state == PLAYING) {
                	if(!isGameOver) {
                    // GameIsOver = CheckColision(Player1, Ghosts, pacman_eatghost);
                    // CheckIfVulnerable(Player1, Ghosts);
                    // CheckScore(Player1, Ghosts);
                    // DrawMap(parede, comida, cafe, seta, seta2);
                    // DrawPacman(Player1, pacmanfinal, curFrame, frameWidth, frameHeight);
                    // DrawGhosts(Ghosts, enemy1, enemy2, enemy3, enemy4, timer2);
                    // al_draw_textf(font, al_map_rgb(255,255,255), 760, 5, 0, "PONTOS");
                    // al_draw_textf(font, al_map_rgb(255,255,255), 800, 150, 0, "%d", Player1.Score+Player1.GhostScore);
					}
                	else {
                    // al_play_sample(pacman_death,1,0,1,ALLEGRO_PLAYMODE_ONCE, NULL);
                    // al_stop_sample_instance(pacman_theme_instance);
                    // finalScore = Player1.Score + Player1.GhostScore;
                    FirstTime = 0;
                    state = GAMEOVER;
                	}
            	}
            	else if (state == GAMEOVER) {
                	// InitPacman(Player1);
                	// InitGhosts(Ghosts);
                	// InitPoseMap();
                	isGameOver = 0;
                	al_draw_text(font, al_map_rgb(255,0,0), WIDTH / 2, 200, ALLEGRO_ALIGN_CENTER, "DERROTA");
                	al_draw_text(font, al_map_rgb(255,255,255), WIDTH / 2, 300, ALLEGRO_ALIGN_CENTER, "pressione ENTER para sair");
                	al_draw_text(font, al_map_rgb(255,255,255), WIDTH / 2, 350, ALLEGRO_ALIGN_CENTER, "ou");
                	al_draw_text(font, al_map_rgb(255,255,255), WIDTH / 2, 400, ALLEGRO_ALIGN_CENTER, "pressione SPACE para ir ao menu");
                	al_draw_textf(font, al_map_rgb(0,0,255), WIDTH / 2, 600, ALLEGRO_ALIGN_CENTER,"high score: %d", player.score);
            	}

            al_flip_display();
            al_clear_to_color(al_map_rgb(0,0,0));
        	}
		}

		else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			done = true;
		}
		if(redraw && al_is_event_queue_empty(event_queue)) {
			redraw = false; 
			if(!isGameOver) {
				al_draw_textf(font, al_map_rgb(255, 0, 255), 5, 5, 0, "Você pode pedir ajuda para os universitários %i vezes. Sua nota atual: %i", player.lives, player.score);
				if (player.score <= 6 & player.score != 10) {
					al_draw_textf(font, al_map_rgb(0, 255, 255), WIDTH / 2, HEIGHT / 2, ALLEGRO_ALIGN_CENTRE, "Parabéns, você conseguiu %i pontos!", player.score);
				}
				if (player.score == 10) {
					al_draw_textf(font, al_map_rgb(0, 255, 255), WIDTH / 2, HEIGHT / 2, ALLEGRO_ALIGN_CENTRE, "Parabéns, você gabaritou!");
				}
			}
			else {
				al_draw_textf(font, al_map_rgb(0, 255, 255), WIDTH / 2, HEIGHT / 2, ALLEGRO_ALIGN_CENTRE, "Game Over. Pontuação final: %i", player.score);
			}
		
			al_flip_display();
			al_clear_to_color(al_map_rgb(0,0,0));
		}
	}
    al_destroy_bitmap(option1);
	al_destroy_bitmap(option2);
	al_destroy_bitmap(option3);
	al_destroy_bitmap(menuimage);
	al_destroy_event_queue(event_queue);
	al_destroy_timer(timer);
	al_destroy_font(font);
	al_destroy_display(display);						//destroy our display object

	return 0;
}

void ChooseThematic(int which) {
    //colocar fundo do display com imagem para esta escolha
	//colocar textos em cima dos blocos de opções
}

void GameInitiation(int theme) {
    
}

void ChooseCharacter (struct Character *player, int which) {
	//pergunta qual jogador ele quer ser "professor", "aluno" ou "aleatorio"
}

void Character(struct Character *player, int charc) { //inicia o personagem
	player->ID = charc;
	player->x = 25;
	player->y = HEIGHT;
	player->lives = 3;
	player->score = 0;
}

void CharacterUpdate(struct Character *player, int charc) {
	for (int i=player->y; i>HEIGHT-25; i--) {
		player->y = i;
	}
}

void Professor(struct Extras *x, struct Extras *y) {
	char choice;
	//id conforme o escolhido
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
	int level = 0;
	level++;

	if (level != 11) {
		if (theme == 1) {
		//print de uma pergunta aleatoria dentre todas as categorias
		}
		if (theme == 2) {
			//print de uma pergunta aleatoria dentre as de professor
		}
		if (theme == 3) {
			//print de uma pergunta aleatoria dentre as de aluno
		}
	}
	else if (level == 11) {
		quest = 0;
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