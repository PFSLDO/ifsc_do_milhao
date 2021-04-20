#include <allegro5/allegro.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <stdio.h>
#include "biblioteca.h"

//VARIIÁVEIS GLOBAIS
const int WIDTH = 700;
const int HEIGHT = 700;
const char QUEST[] = "/Users/pamela_fialho/Documents/GitHub/ifsc_do_milhao/ifsc_do_milhao/questions.csv";
const char ALT[] = "/Users/pamela_fialho/Documents/GitHub/ifsc_do_milhao/ifsc_do_milhao/alternatives.csv";
FILE  *fileques;
FILE  *filealt;
enum KEYS {SPACE, NUM1, NUM2, NUM3, ESC, P, A, H};
enum STATE {MENU, CHOOSE_CHARACTER, CHOOSE_THEMATIC, PLAYING, GAMEOVER, WON};

//PROTÓTIPO DE FUNÇÕES
//Relacionadas aos personagens
void ChooseCharacter (struct Character *player, int which); //escolha do personagem do jogador
void ChooseThematic(int which); //modalidade das perguntas
void Character(struct Character *player); //inicia o personagem do jogador
void Professor(struct Extras *x, struct Extras *y); //aparição de um professor
void Interviewer(struct Extras *interviewer, struct Question *quest); //aparição de um entrevistador

//Relaciondas ao jogo
void NewQuestion(struct Question *quest); //gera uma pergunta nova
void Answer(struct Character *player, struct Question *quest); //analise da resposta

//Define como será mostrada uma mensagem de erro
void Error(char *text){
	al_show_native_message_box(NULL,"ERRO", "Ocorreu o seguinte erro e o programa sera finalizado:",text,NULL,ALLEGRO_MESSAGEBOX_ERROR);
}

int main(void) {
	//VARIÁVEIS PRIMITIVAS
	//Construção da estrutura do jogo
	const int FPS = 60;
	bool done = false;
	bool redraw = true;
	bool isGameOver = false;
	bool FirstTime = true;
	int state = MENU; //inicia no menu
	int n = 0; //controla o número de linhas lidas do arquivo csv

	//Leitura do teclado
	bool keys[8] = {false, false, false, false, false, false, false, false};

	//Variáveis para leitura arquivos csv
	char titleques[100], Questions[60];
	int QuestID[60];
	char titlealt[100], Alternatives[180];
	int AltID[180];

	//Variáveis de objeto
	struct Character player; //cria o jogador
	struct Extras interviewer; //cria a entrevistador
	struct Extras x; //cria o professor x
	struct Extras y; //cria o professor y
	struct Question quest; //cria uma estrutura para organizar a resposta

	//Variáveis do Allegro
	ALLEGRO_DISPLAY *display = NULL;
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	ALLEGRO_TIMER *timer = NULL;
	ALLEGRO_FONT *font = NULL;
	ALLEGRO_FONT *fontG = NULL;

	//Variáveis do Allegro relacionadas a imagem
	ALLEGRO_BITMAP *menuimage = NULL;
	ALLEGRO_BITMAP *charactermenu = NULL;
	ALLEGRO_BITMAP *thematicmenu = NULL;

	//Variáveis do Allegro relacionadas a audio
	ALLEGRO_SAMPLE *game_theme = NULL;
    ALLEGRO_SAMPLE_INSTANCE *game_theme_instance = NULL;

	//FUNÇÕES DE INICIALIZAÇÃO
	if (!al_init()) { 
        Error("Falha ao inicializar o Allegro");
        return -1;
    }

	if(!al_install_audio()) {
        return -1;
        Error("Erro ao iniciar os sistemas de Audio!\n");
    }

    if(!al_init_acodec_addon()) {
        return -1;
        Error("Erro ao iniciar os sistemas de codec de Audio!\n");
    }

    if (!al_reserve_samples(1)) {
        return -1;
        Error("Erro ao reservar as samples de Audio!\n");
    }

	display = al_create_display(WIDTH, HEIGHT); //Cria o objeto display
	
	if(!display) {
		Error("Falha ao criar o display");									//test display object
		return -1;
	}

	if(!al_init_primitives_addon()) {
        return -1;
        Error("Erro inciar os addons primitivos!\n");
    }

    if(!al_install_keyboard()) {
        return -1;
        Error("Erro instalar/iniciar o teclado!\n");
    }

    if(!al_init_image_addon()) {
        return -1;
        Error("Erro iniciar o addon de imagem!\n");
    }

	al_init_primitives_addon();
	al_init_font_addon();
	al_init_ttf_addon();
	al_init_image_addon();

	//CONFIGURA O TÍTULO DA JANELA
    al_set_window_title(display, "IFSC do Milhão");

	//CARREGA OS ARQUIVOS UTILIZADOS
	//Carrega os arquivos relacionados a imagem
	menuimage = al_load_bitmap("/Users/pamela_fialho/Documents/GitHub/ifsc_do_milhao/ifsc_do_milhao/menu_image.png"); //carrega a imagem do menu
	charactermenu = al_load_bitmap("/Users/pamela_fialho/Documents/GitHub/ifsc_do_milhao/ifsc_do_milhao/menu_character.png"); //carrega a imagem do menu de personagens
	thematicmenu = al_load_bitmap("/Users/pamela_fialho/Documents/GitHub/ifsc_do_milhao/ifsc_do_milhao/menu_thematic.png"); //carrega a imagem do menu de temáticas
	
	//Carrega os arquivos relacionados a fonte
	font = al_load_font("/Users/pamela_fialho/Documents/GitHub/Listas_de_Exercicios_Programacao_em_Linguagem_C/atividade_expansao_dos_cometas/arial.ttf", 20, 0);
	fontG = al_load_font("/Users/pamela_fialho/Documents/GitHub/Listas_de_Exercicios_Programacao_em_Linguagem_C/atividade_expansao_dos_cometas/arial.ttf", 30, 0);

	//Carrega os arquivos relacionados a audio
	al_reserve_samples(2);//Reserva 2 samples, mais do que o suficiente para o que vai ser usado
    game_theme = al_load_sample("Audio/game_theme.wav");//carrega o tema
    game_theme_instance = al_create_sample_instance(game_theme);//cria uma sample instance e coloca o tema dentro
    al_set_sample_instance_playmode(game_theme_instance, ALLEGRO_PLAYMODE_LOOP);//configura o playmode da sample instance, nesse caso loop
    al_attach_sample_instance_to_mixer(game_theme_instance, al_get_default_mixer());//da o "atach" da sample_instance ao mixer

	//Faz a leitura dos arquivos com as perguntas
	fileques = fopen(QUEST, "r");
    if (fileques == NULL) {
        Error("Erro ao abrir o arquivo de perguntas.\n");
        return 0;
    }
    for(int i=0; i<80; i++) {
        fgets(titleques, sizeof(titleques),fileques);
        n=fscanf(fileques, "%d, %s", &QuestID[i], &Questions[i]);
        if (n==EOF) {
            break;
        }
    }
    fclose(fileques);

	//Faz a leitura dos arquivos com as respostas
	filealt = fopen(ALT, "r");
    if (filealt == NULL) {
        Error("Erro ao abrir o arquivo de perguntas.\n");
        return 0;
    }
    for(int i=0; i<180; i++) {
        fgets(titlealt, sizeof(titlealt),filealt);
        n=fscanf(filealt, "%d, %s", &AltID[i], &Alternatives[i]);
    }
    fclose(fileques);
	
	//CONTROLE DE TEMPO DO JOGO
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
		if(ev.type == ALLEGRO_EVENT_KEY_DOWN) { //Verifica qual tecla é apertada quando alguma é ativada
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
		else if(ev.type == ALLEGRO_EVENT_KEY_UP) { //Registra que a tecla foi solta
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
			if (state == MENU) { //Verifica o teclado na tela do menu principal
                if(keys[SPACE]) {
                    state = CHOOSE_CHARACTER;
				}
                if(keys[ESC]) {
                    done = true;
				}
            }
			else if (state == CHOOSE_CHARACTER) { //Verifica o teclado na tela do menu de escolha do personagem
				if(keys[A]) { //Caso escolha jogar com o aluno
					ChooseCharacter(&player, 1);
					state = CHOOSE_THEMATIC; //Avança para a tela da escolha da temática
				}
				if(keys[P]) { //Caso escolha jogar com a professora
					ChooseCharacter(&player, 2);
					state = CHOOSE_THEMATIC; //Avança para a tela da escolha da temática
				}
                if(keys[ESC]) {
					done = true;
				}
            }
			else if (state == CHOOSE_THEMATIC) { //Verifica o teclado na tela do menu de escolha da temática
				for(int i=1; i<4; i++) {
					if(keys[i]) {
						ChooseThematic(i); //Escolhe a temática correspondente ao que foi lido pelo teclado
						state = PLAYING; //Avança para a tela da jogada
					}
				}
                if(keys[ESC]) {
					done = true;
				}
            }
			else if (state == PLAYING) {
                if(FirstTime) { //Roda apenas ao entrar na jogada pela primeira vez
                    Character(&player); //Inicia o personagem do jogador
					//help professor
					Interviewer(&interviewer, &quest); //Inicia o entrevistador
                    FirstTime = false; //Registra que a partir deste momento, não será a primeira vez na rodade
                }
				NewQuestion(&quest); //Chama a função que printa nova pergunta
				for (int i=1; i<4; i++) {
					if (keys[i]) {
						quest.player_answer = i; //Guarda a reposta do jogador no struct
						Answer(&player, &quest); //Chama a função que verifica a resposta do jogador
					}
				}
				if(player.score == 10) { //Se o jogador alcancar 10 pontos, terá ganho o jogo
					state = WON;
				}
				if(player.score < 6 && quest.num == 10) { //Se o jogador completar as 10 perguntas e não atingir no mínimo 6 pontos, ele perde
					state = GAMEOVER;
				}
                if(keys[ESC]) {
					done = true;
				}
            }
			else if (state == GAMEOVER) { //Faz a leitura do que o jogador escolherá fazer depois de perder
                if(keys[ESC]) {
					done = true;
				}
                else if (keys[SPACE]) {
                    state = MENU;
                }
            }
			else if (state == WON) { //Faz a leitura do que o jogador escolherá fazer depois de ganhar
                if(keys[ESC]) {
					done = true;
				}
                else if (keys[SPACE]) {
                    state = MENU;
                }
            }

			if(redraw && al_is_event_queue_empty(event_queue)) {
            	redraw = false;
				if(!isGameOver) {
					al_draw_textf(font, al_map_rgb(255, 255, 255), 5, 5, 0, "Você pode pedir ajuda para os universitários %i vezes. Sua nota atual: %i", player.lives, player.score);
				}
				if (state == MENU) {
					al_draw_bitmap(menuimage,0,0,0); //imagem de fundo
					al_draw_text(font, al_map_rgb(255,255,255), WIDTH/2, 290, ALLEGRO_ALIGN_CENTER, "COMO JOGAR:");
                	al_draw_text(font, al_map_rgb(255,255,255), WIDTH/2, 320, ALLEGRO_ALIGN_CENTER, "Utilize as teclas do seu computador para fazer as escolhas!");
                	al_draw_text(font, al_map_rgb(255,255,255), WIDTH/2, 340, ALLEGRO_ALIGN_CENTER, "A escolha da resposta é feita com as teclas 1, 2 e 3");
                	al_draw_text(font, al_map_rgb(255,255,255), WIDTH/2, 360, ALLEGRO_ALIGN_CENTER, "Você poderá escolher o personagem e a temática das perguntas a seguir");
                	al_draw_text(font, al_map_rgb(255,255,255), WIDTH/2, 390, ALLEGRO_ALIGN_CENTER, "BOA SORTE!");
					al_draw_text(font, al_map_rgb(255,255,255), WIDTH/2, 470, ALLEGRO_ALIGN_CENTER, "ESC PARA SAIR DO JOGO");
                	al_draw_text(font, al_map_rgb(255,255,255), WIDTH/2, 597, ALLEGRO_ALIGN_CENTER, "SPACE PARA COMEÇAR A JOGAR");
            	}
				else if (state == CHOOSE_CHARACTER) {
					al_draw_bitmap(charactermenu,0,0,0); //imagem menu de personagem
					al_draw_text(font, al_map_rgb(255,255,255), WIDTH/2, 60, ALLEGRO_ALIGN_CENTER, "ESCOLHA, COM O TECLADO, SEU PERSONAGEM");
					al_draw_text(font, al_map_rgb(255,255,255), WIDTH/2, 470, ALLEGRO_ALIGN_CENTER, "A) PARA JOGAR COM ALUNO");
                	al_draw_text(font, al_map_rgb(255,255,255), WIDTH/2, 597, ALLEGRO_ALIGN_CENTER, "P) PARA JOGAR COM PROFESSORA");
				}
				else if (state == CHOOSE_THEMATIC) {
					al_draw_bitmap(thematicmenu,0,0,0); //imagem menu da tematica
					al_draw_text(fontG, al_map_rgb(255,255,255), WIDTH/2, 90, ALLEGRO_ALIGN_CENTER, "ESCOLHA, COM O TECLADO, UMA");
					al_draw_text(fontG, al_map_rgb(255,255,255), WIDTH/2, 130, ALLEGRO_ALIGN_CENTER, "TEMÁTICA PARA JOGAR");
					al_draw_text(font, al_map_rgb(255,255,255), WIDTH/2, 275, ALLEGRO_ALIGN_CENTER, "1) PARA JOGAR COM A TEMÁTICA IFSC");
                	al_draw_text(font, al_map_rgb(255,255,255), WIDTH/2, 435, ALLEGRO_ALIGN_CENTER, "2) PARA JOGAR COM A TEMÁTICA");
					al_draw_text(font, al_map_rgb(255,255,255), WIDTH/2, 455, ALLEGRO_ALIGN_CENTER, "CONHECIMENTOS GERAIS");
					al_draw_text(font, al_map_rgb(255,255,255), WIDTH/2, 600, ALLEGRO_ALIGN_CENTER, "3) PARA JOGAR COM A TODAS AS TEMÁTICA");
				}
            	else if (state == PLAYING) {
                	if(!isGameOver) {
                    	al_draw_textf(font, al_map_rgb(255,255,255), 760, 5, 0, "PONTOS");
                    	al_draw_textf(font, al_map_rgb(255,255,255), 800, 150, 0, "%d", player.score);
					}
                	else {
                    	FirstTime = true;
                    	state = GAMEOVER;
                	}
            	}
            	else if (state == GAMEOVER) {
					al_clear_to_color(al_map_rgb(0,0,0));
                	isGameOver = true;
                	al_draw_text(font, al_map_rgb(255,0,0), WIDTH / 2, 200, ALLEGRO_ALIGN_CENTER, "DERROTA");
                	al_draw_text(font, al_map_rgb(255,255,255), WIDTH / 2, 300, ALLEGRO_ALIGN_CENTER, "pressione ESC para sair");
                	al_draw_text(font, al_map_rgb(255,255,255), WIDTH / 2, 350, ALLEGRO_ALIGN_CENTER, "ou");
                	al_draw_text(font, al_map_rgb(255,255,255), WIDTH / 2, 400, ALLEGRO_ALIGN_CENTER, "pressione SPACE para ir ao menu");
                	al_draw_textf(font, al_map_rgb(0,0,255), WIDTH / 2, 600, ALLEGRO_ALIGN_CENTER,"Você conseguiu %d pontos", player.score);
            	}
				else if (state == WON) { //Faz a leitura do que o jogador escolherá fazer depois de ganhar
					isGameOver = true;
					al_clear_to_color(al_map_rgb(0,0,0));
					if (player.score >= 6 & player.score != 10) {
						al_draw_textf(font, al_map_rgb(0, 255, 255), WIDTH / 2, HEIGHT / 2, ALLEGRO_ALIGN_CENTRE, "Parabéns, você conseguiu %i pontos!", player.score);
						al_draw_text(font, al_map_rgb(255,255,255), WIDTH / 2, 300, ALLEGRO_ALIGN_CENTER, "pressione ESC para sair");
                		al_draw_text(font, al_map_rgb(255,255,255), WIDTH / 2, 350, ALLEGRO_ALIGN_CENTER, "ou");
                		al_draw_text(font, al_map_rgb(255,255,255), WIDTH / 2, 400, ALLEGRO_ALIGN_CENTER, "pressione SPACE para ir ao menu");
					}
					else if (player.score == 10) {
						al_draw_text(font, al_map_rgb(255,0,0), WIDTH / 2, 200, ALLEGRO_ALIGN_CENTER, "VOCÊ GABARITOU!");
                		al_draw_text(font, al_map_rgb(255,255,255), WIDTH / 2, 300, ALLEGRO_ALIGN_CENTER, "pressione ESC para sair");
                		al_draw_text(font, al_map_rgb(255,255,255), WIDTH / 2, 350, ALLEGRO_ALIGN_CENTER, "ou");
                		al_draw_text(font, al_map_rgb(255,255,255), WIDTH / 2, 400, ALLEGRO_ALIGN_CENTER, "pressione SPACE para ir ao menu");
					}
            	}

            al_flip_display();
            al_clear_to_color(al_map_rgb(0,0,0));
        	}
		}
	}
	al_destroy_bitmap(menuimage);
	al_destroy_bitmap(charactermenu);
	al_destroy_bitmap(thematicmenu);
	al_destroy_event_queue(event_queue);
	al_destroy_timer(timer);
	al_destroy_font(font);
	al_destroy_display(display);						//destroy our display object

	return 0;
}

void ChooseCharacter (struct Character *player, int which) {
	if (which == 1) {
		player->ID = STUDENT;
	}
	else if (which == 2) {
		player->ID = PROFESSOR;
	}
}

void ChooseThematic(int which) {
    //colocar fundo do display com imagem para esta escolha
	//colocar textos em cima dos blocos de opções
}

void Character(struct Character *player) { //inicia o personagem
	if (player->ID == STUDENT) {
		//imagem do estudante
	}
	if (player->ID == PROFESSOR) {
		//imagem do estudante
	}
	player->x = 25;
	player->y = HEIGHT;
	player->lives = 3;
	player->score = 0;
}

void Professor(struct Extras *x, struct Extras *y) {
	// char choice;
	// //id conforme o escolhido
	// x->ID = choice;
	// x->x = 25;
	// x->y = HEIGHT;
}

void Interviewer(struct Extras *interviewer, struct Question *quest) {
	if ((rand() % 2 + 1) == 1) {
		interviewer->ID = INTERVIEWER_PAMELA;
		//carrega imagem pamela
	}
	else {
		interviewer->ID = INTERVIEWER_VALTER;
		//carrega imagem valter
	}
}

void NewQuestion(struct Question *quest) { //implementar logica para nao repetir pergunta
	
}

void Answer(struct Character *player, struct Question *quest) {
	if (quest->player_answer == quest->answer) {
		player->score++;
	}
}