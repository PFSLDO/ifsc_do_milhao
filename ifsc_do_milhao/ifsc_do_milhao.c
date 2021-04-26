#include <allegro5/allegro.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "objects.h"

//VARIIÁVEIS GLOBAIS
const int WIDTH = 700;
const int HEIGHT = 700;
const char QUEST[] = "/Users/pamela_fialho/Documents/GitHub/ifsc_do_milhao/ifsc_do_milhao/csv_files/questions.csv";
const char ALT[] = "/Users/pamela_fialho/Documents/GitHub/ifsc_do_milhao/ifsc_do_milhao/csv_files/alternatives.csv";
const char TIPS[] = "/Users/pamela_fialho/Documents/GitHub/ifsc_do_milhao/ifsc_do_milhao/csv_files/tips.csv";
FILE  *fileques;
FILE  *filealt;
FILE  *filetips;
enum KEYS {SPACE, NUM1, NUM2, NUM3, ESC, H, S, P, A, B, C};
enum STATE {MENU, CHOOSE_CHARACTER, CHOOSE_THEMATIC, PLAYING, GAMEOVER, WON};

//PROTÓTIPO DE FUNÇÕES
//Relacionadas aos personagens
void Character(struct Character *player); //inicia o personagem do jogador
void Interviewer(struct Extras *interviewer, struct Question *quest); //aparição de um entrevistador

//Relaciondas ao jogo
void NewQuestion(struct Question *quest, int questID[60], int questIDans[180]); //gera uma pergunta nova
void Answer(struct Character *player, struct Question *quest, int verifyID[180], ALLEGRO_SAMPLE *right_answer, ALLEGRO_SAMPLE *wrong_answer); //analise da resposta

//Define como será mostrada uma mensagem de erro
void Error(char *text) {
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
	bool Wait = false;
	bool NeedHelp = false;
	int state = MENU; //inicia no menu

	//Leitura do teclado
	bool keys[11] = {false, false, false, false, false, false, false, false, false, false, false};

	//Variáveis para leitura arquivos csv
	char titleques[100], questions[60][100];
	int questID[60];

	char titlealt[100], alternatives[180][100];
	int altID[180], verifyID[180], questIDans[180];

	char titletips[100], tips[60][100];
	int questIDtips[60], tipsID[60];

	//Variáveis de objeto
	struct Character player; //cria o jogador
	struct Extras interviewer; //cria a entrevistador
	struct Question quest; //cria uma estrutura para organizar a resposta

	//Variáveis do Allegro
	ALLEGRO_DISPLAY *display = NULL;
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	ALLEGRO_TIMER *timer = NULL;
	ALLEGRO_FONT *fontP = NULL;
	ALLEGRO_FONT *fontM = NULL;
	ALLEGRO_FONT *fontG = NULL;

	//Variáveis do Allegro relacionadas a imagem
	ALLEGRO_BITMAP *menuimage = NULL;
	ALLEGRO_BITMAP *charactermenu = NULL;
	ALLEGRO_BITMAP *thematicmenu = NULL;
	ALLEGRO_BITMAP *menuplaying = NULL;
	ALLEGRO_BITMAP *end = NULL;
	ALLEGRO_BITMAP *student = NULL;
	ALLEGRO_BITMAP *professor = NULL;
	ALLEGRO_BITMAP *pamela = NULL;
	ALLEGRO_BITMAP *valter = NULL;

	//Variáveis do Allegro relacionadas a audio
	ALLEGRO_SAMPLE *game_theme = NULL;
	ALLEGRO_SAMPLE *right_answer = NULL;
	ALLEGRO_SAMPLE *wrong_answer = NULL;
    ALLEGRO_SAMPLE_INSTANCE *game_theme_instance = NULL;

	//FUNÇÕES DE INICIALIZAÇÃO
	srand(time(0)); //aleatoriedade de rand()

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
		Error("Falha ao criar o display");
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
	menuimage = al_load_bitmap("/Users/pamela_fialho/Documents/GitHub/ifsc_do_milhao/ifsc_do_milhao/images/menu_image.png"); //carrega a imagem do menu
	charactermenu = al_load_bitmap("/Users/pamela_fialho/Documents/GitHub/ifsc_do_milhao/ifsc_do_milhao/images/menu_character.png"); //carrega a imagem do menu de personagens
	thematicmenu = al_load_bitmap("/Users/pamela_fialho/Documents/GitHub/ifsc_do_milhao/ifsc_do_milhao/images/menu_thematic.png"); //carrega a imagem do menu de temáticas
	menuplaying = al_load_bitmap("/Users/pamela_fialho/Documents/GitHub/ifsc_do_milhao/ifsc_do_milhao/images/menu_playing.png"); //carrega a imagem do menu da jogada
	end = al_load_bitmap("/Users/pamela_fialho/Documents/GitHub/ifsc_do_milhao/ifsc_do_milhao/images/menu_final.png"); //carrega a imagem do menu do final do jogo
	student = al_load_bitmap("/Users/pamela_fialho/Documents/GitHub/ifsc_do_milhao/ifsc_do_milhao/images/student.png"); //carrega a imagem do jogador estudante
	professor = al_load_bitmap("/Users/pamela_fialho/Documents/GitHub/ifsc_do_milhao/ifsc_do_milhao/images/professor.png"); //carrega a imagem do jogador professor
	valter = al_load_bitmap("/Users/pamela_fialho/Documents/GitHub/ifsc_do_milhao/ifsc_do_milhao/images/valter.png"); //carrega a imagem do jogador professor
	pamela = al_load_bitmap("/Users/pamela_fialho/Documents/GitHub/ifsc_do_milhao/ifsc_do_milhao/images/pamela.png"); //carrega a imagem do jogador professor

	//Carrega os arquivos relacionados a fonte
	fontP = al_load_font("/Users/pamela_fialho/Documents/GitHub/Listas_de_Exercicios_Programacao_em_Linguagem_C/atividade_expansao_dos_cometas/vcr.ttf", 13, 0);
	fontM = al_load_font("/Users/pamela_fialho/Documents/GitHub/Listas_de_Exercicios_Programacao_em_Linguagem_C/atividade_expansao_dos_cometas/vcr.ttf", 18, 0);
	fontG = al_load_font("/Users/pamela_fialho/Documents/GitHub/Listas_de_Exercicios_Programacao_em_Linguagem_C/atividade_expansao_dos_cometas/vcr.ttf", 28, 0);

	//Carrega os arquivos relacionados a audio
	al_reserve_samples(4);//Reserva 2 samples, mais do que o suficiente para o que vai ser usado
    game_theme = al_load_sample("/Users/pamela_fialho/Documents/GitHub/Listas_de_Exercicios_Programacao_em_Linguagem_C/atividade_expansao_dos_cometas/music.wav");//carrega o tema
    right_answer = al_load_sample("/Users/pamela_fialho/Documents/GitHub/Listas_de_Exercicios_Programacao_em_Linguagem_C/atividade_expansao_dos_cometas/right.wav");//carrega o tema
    wrong_answer = al_load_sample("/Users/pamela_fialho/Documents/GitHub/Listas_de_Exercicios_Programacao_em_Linguagem_C/atividade_expansao_dos_cometas/wrong.wav");//carrega o tema
	game_theme_instance = al_create_sample_instance(game_theme);//cria uma sample instance e coloca o tema dentro
    al_set_sample_instance_playmode(game_theme_instance, ALLEGRO_PLAYMODE_LOOP);//configura o playmode da sample instance, nesse caso loop
    al_attach_sample_instance_to_mixer(game_theme_instance, al_get_default_mixer());//da o "atach" da sample_instance ao mixer

	//Faz a leitura dos arquivos com as perguntas
	fileques = fopen(QUEST, "r");
    if (fileques == NULL) {
        Error("Erro ao abrir o arquivo de perguntas.\n");
        return 0;
    }
    for (int i=0; i<60; i++) {
        fgets(titleques, sizeof(titleques),fileques);
        fscanf(fileques, "%d,%[^,]", &questID[i], &questions[i][0]);
    }
    fclose(fileques);

	//Faz a leitura dos arquivos com as respostas
	filealt = fopen(ALT, "r");
    if (filealt == NULL) {
        Error("Erro ao abrir o arquivo de perguntas.\n");
        return 0;
    }
    for (int i=0; i<180; i++) {
        fgets(titlealt, sizeof(titlealt),filealt);
        fscanf(filealt, "%d,%d,%[^,],%d", &questIDans[i], &altID[i], &alternatives[i][0], &verifyID[i]);
    }
    fclose(fileques);

	//Faz a leitura dos arquivos com as dicas
	filetips = fopen(TIPS, "r");
    if (filetips == NULL) {
        Error("Erro ao abrir o arquivo de perguntas.\n");
        return 0;
    }
    for (int i=0; i<180; i++) {
        fgets(titletips, sizeof(titletips),filetips);
        fscanf(filetips, "%d,%d,%[^,]", &questIDtips[i], &tipsID[i], &tips[i][0]);
    }
    fclose(filetips);
	
	//CONTROLE DE TEMPO DO JOGO
	event_queue = al_create_event_queue();
	timer = al_create_timer(1.0 / FPS);

	al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_timer_event_source(timer));

	al_start_timer(timer); //Inicia o timer
    al_play_sample_instance(game_theme_instance); //Inicia a trilha sonora do jogo
	
	while(!done) {
		ALLEGRO_EVENT ev;
		al_wait_for_event(event_queue, &ev);
		if(ev.type == ALLEGRO_EVENT_KEY_DOWN) { //Verifica qual tecla é apertada quando alguma é ativada
			switch(ev.keyboard.keycode) {
				case ALLEGRO_KEY_SPACE:
                	keys[SPACE] = true;
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
				case ALLEGRO_KEY_ESCAPE:
                	keys[ESC] = true;
                	break;
				case ALLEGRO_KEY_H:
                	keys[H] = true;
                	break;
				case ALLEGRO_KEY_S:
                	keys[S] = true;
                	break;
				case ALLEGRO_KEY_P:
                	keys[P] = true;
                	break;
				case ALLEGRO_KEY_A:
                	keys[A] = true;
                	break;
				case ALLEGRO_KEY_B:
                	keys[B] = true;
                	break;
				case ALLEGRO_KEY_C:
                	keys[C] = true;
                	break;
			}
		}
		else if(ev.type == ALLEGRO_EVENT_KEY_UP) { //Registra que a tecla foi solta
            switch(ev.keyboard.keycode) {
                case ALLEGRO_KEY_ESCAPE:
					keys[ESC] = false;
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
					keys[SPACE]=false;
				}
                if(keys[ESC]) {
                    done = true;
					keys[ESC]=false;
				}
            }
			else if (state == CHOOSE_CHARACTER) { //Verifica o teclado na tela do menu de escolha do personagem
				if(keys[S]) { //Caso escolha jogar com o aluno
					player.ID = STUDENT;
					state = CHOOSE_THEMATIC; //Avança para a tela da escolha da temática
					keys[S]=false;
				}
				if(keys[P]) { //Caso escolha jogar com a professora
					player.ID = PROFESSOR;
					state = CHOOSE_THEMATIC; //Avança para a tela da escolha da temática
					keys[P]=false;
				}
                if(keys[ESC]) {
					done = true;
					keys[ESC]=false;
				}
            }
			else if (state == CHOOSE_THEMATIC) { //Verifica o teclado na tela do menu de escolha da temática
				if(keys[NUM1]) { //Caso escolha a temática 1
					quest.thematic = 1;
					state = PLAYING; //Avança para a tela da escolha da temática
					keys[NUM1]=false;
				}
				if(keys[NUM2]) { //Caso escolha a temática 2
					quest.thematic = 2;
					state = PLAYING; //Avança para a tela da escolha da temática
					keys[NUM2]=false;
				}
				if(keys[NUM3]) { //Caso escolha todas as temáticas
					quest.thematic = 3;
					state = PLAYING; //Avança para a tela da escolha da temática
					keys[NUM3]=false;
				}
                if(keys[ESC]) {
					done = true;
					keys[ESC]=false;
				}
            }
			else if (state == PLAYING) {
                if(FirstTime) { //Roda apenas ao entrar na jogada pela primeira vez
                    Character(&player); //Inicia o personagem do jogador
					quest.num = 0; //Zera a questão atual
					player.score = 0; //Zera a pontuação do usuário
					player.lives = 3; //Permite que o usuário peça ajuda 3 vezes
					Wait = false;
                    FirstTime = false; //Registra que a partir deste momento, não será a primeira vez na rodada
                }
				if(!Wait) {
					NewQuestion(&quest, questID, questIDans); //Chama a função que printa nova pergunta
				}
				Interviewer(&interviewer, &quest); //Inicia o entrevistador
				Wait = true;
				if(keys[A]) {
					quest.player_answer = 0; //Registra a escolha do usuário
					Answer(&player, &quest, verifyID, right_answer, wrong_answer); //Chama a função que verifica a resposta do jogador
					quest.num++; //Contabiliza o valor da questão atual
					Wait = false;
					keys[A]=false;
					NeedHelp = false;
				}
				if(keys[B]) {
					quest.player_answer = 1; //Registra a escolha do usuário
					Answer(&player, &quest, verifyID, right_answer, wrong_answer); //Chama a função que verifica a resposta do jogador
					quest.num++; //Contabiliza o valor da questão atual
					Wait = false;
					keys[B]=false;
					NeedHelp = false;
				}
				if(keys[C]) {
					quest.player_answer = 2; //Registra a escolha do usuário
					Answer(&player, &quest, verifyID, right_answer, wrong_answer); //Chama a função que verifica a resposta do jogador
					quest.num++; //Contabiliza o valor da questão atual
					Wait = false;
					keys[C]=false;
					NeedHelp = false;
				}
				if(keys[H] && player.lives>0) {
					player.lives = player.lives - 1;
					NeedHelp = true;
					keys[H]=false;
				}
				if (player.score >= 6 && quest.num == 10) { //Se o jogador completar as 10 perguntas e atingir uma nota igual ou acima de 6, ele ganha o jogo
					state = WON;
				}
				if(player.score < 6 && quest.num == 10) { //Se o jogador completar as 10 perguntas e não atingir no mínimo 6 pontos, ele perde
					state = GAMEOVER;
				}
                if(keys[ESC]) {
					done = true;
					keys[ESC]=false;
				}
            }
			else if (state == GAMEOVER) { //Faz a leitura do que o jogador escolherá fazer depois de perder
				if(keys[ESC]) {
					done = true;
					keys[ESC]=false;
				}
                else if (keys[SPACE]) { //Volta para o menu inicial caso o jogador queira jogar novamente
                    state = MENU;
					isGameOver = false;
					keys[SPACE]=false;
                }
            }
			else if (state == WON) { //Faz a leitura do que o jogador escolherá fazer depois de ganhar
				if(keys[ESC]) {
					done = true;
					keys[ESC]=false;
				}
                else if (keys[SPACE]) { //Volta para o menu inicial caso o jogador queira jogar novamente
                    state = MENU;
					isGameOver = false;
					keys[SPACE]=false;
                }
            }

			if(redraw && al_is_event_queue_empty(event_queue)) {
            	redraw = false;
				if (state == MENU) {
					al_draw_bitmap(menuimage,0,0,0); //imagem de fundo
					al_draw_text(fontM, al_map_rgb(255,255,255), WIDTH/2, 290, ALLEGRO_ALIGN_CENTER, "COMO JOGAR:");
                	al_draw_text(fontM, al_map_rgb(255,255,255), WIDTH/2, 320, ALLEGRO_ALIGN_CENTER, "Utilize as teclas do seu computador para fazer as escolhas!");
                	al_draw_text(fontM, al_map_rgb(255,255,255), WIDTH/2, 340, ALLEGRO_ALIGN_CENTER, "A escolha da resposta é feita com as teclas A, B e C");
                	al_draw_text(fontM, al_map_rgb(255,255,255), WIDTH/2, 360, ALLEGRO_ALIGN_CENTER, "Você tem direito de pedir ajuda 3 vezes");
                	al_draw_text(fontM, al_map_rgb(255,255,255), WIDTH/2, 390, ALLEGRO_ALIGN_CENTER, "BOA SORTE!");
					al_draw_text(fontM, al_map_rgb(255,255,255), WIDTH/2, 473, ALLEGRO_ALIGN_CENTER, "ESC PARA SAIR DO JOGO");
                	al_draw_text(fontM, al_map_rgb(255,255,255), WIDTH/2, 600, ALLEGRO_ALIGN_CENTER, "SPACE PARA COMEÇAR A JOGAR");
            	}
				else if (state == CHOOSE_CHARACTER) {
					al_draw_bitmap(charactermenu,0,0,0); //imagem menu de personagem
					al_draw_text(fontM, al_map_rgb(255,255,255), WIDTH/2, 60, ALLEGRO_ALIGN_CENTER, "ESCOLHA, COM O TECLADO, SEU PERSONAGEM");
					al_draw_text(fontM, al_map_rgb(255,255,255), WIDTH/2, 473, ALLEGRO_ALIGN_CENTER, "S) PARA JOGAR COM ALUNO");
                	al_draw_text(fontM, al_map_rgb(255,255,255), WIDTH/2, 600, ALLEGRO_ALIGN_CENTER, "P) PARA JOGAR COM PROFESSORA");
				}
				else if (state == CHOOSE_THEMATIC) {
					al_draw_bitmap(thematicmenu,0,0,0); //imagem menu da tematica
					al_draw_text(fontG, al_map_rgb(255,255,255), WIDTH/2, 90, ALLEGRO_ALIGN_CENTER, "ESCOLHA, COM O TECLADO, UMA");
					al_draw_text(fontG, al_map_rgb(255,255,255), WIDTH/2, 130, ALLEGRO_ALIGN_CENTER, "TEMÁTICA PARA JOGAR");
					al_draw_text(fontM, al_map_rgb(255,255,255), WIDTH/2, 278, ALLEGRO_ALIGN_CENTER, "1) PARA JOGAR COM A TEMÁTICA IFSC");
                	al_draw_text(fontM, al_map_rgb(255,255,255), WIDTH/2, 438, ALLEGRO_ALIGN_CENTER, "2) PARA JOGAR COM A TEMÁTICA");
					al_draw_text(fontM, al_map_rgb(255,255,255), WIDTH/2, 458, ALLEGRO_ALIGN_CENTER, "CONHECIMENTOS GERAIS");
					al_draw_text(fontM, al_map_rgb(255,255,255), WIDTH/2, 603, ALLEGRO_ALIGN_CENTER, "3) PARA JOGAR COM A TODAS AS TEMÁTICA");
				}
            	else if (state == PLAYING) {
					al_draw_bitmap(menuplaying,0,0,0); //imagem de fundo
					if (player.ID == STUDENT) {
						al_draw_scaled_bitmap(student, 0, 0, 512, 512, player.x, player.y, 320, 320, 0);
					}
					else if (player.ID == PROFESSOR) {
						al_draw_scaled_bitmap(professor, 0, 0, 512, 512, player.x, player.y, 320, 320, 0);
					}
					if (interviewer.ID == INTERVIEWER_VALTER) {
						al_draw_scaled_bitmap(valter, 0, 0, 512, 512, interviewer.x, interviewer.y, 320, 320, 0);
					}
					else if (interviewer.ID == INTERVIEWER_PAMELA) {
						al_draw_scaled_bitmap(pamela, 0, 0, 512, 512, interviewer.x, interviewer.y, 320, 320, 0);
					}
                	if(!isGameOver) {
						al_draw_textf(fontP, al_map_rgb(255,255,255), WIDTH / 2, 100, ALLEGRO_ALIGN_CENTER,"%s", questions[quest.question_loc]);
						al_draw_textf(fontM, al_map_rgb(255,255,255), WIDTH / 2 - 40, 670, ALLEGRO_ALIGN_CENTER,"Rodada atual: %d", quest.num + 1);
						al_draw_textf(fontG, al_map_rgb(255,255,255), 32, 380, ALLEGRO_ALIGN_CENTER,"A");
						al_draw_textf(fontG, al_map_rgb(255,255,255), 32, 490, ALLEGRO_ALIGN_CENTER,"B");
						al_draw_textf(fontG, al_map_rgb(255,255,255), 32, 600, ALLEGRO_ALIGN_CENTER,"C");
						al_draw_textf(fontP, al_map_rgb(255,255,255), WIDTH / 2 - 40, 380, ALLEGRO_ALIGN_CENTER,"%s", alternatives[quest.first_answer_loc]);
						al_draw_textf(fontP, al_map_rgb(255,255,255), WIDTH / 2 - 40, 490, ALLEGRO_ALIGN_CENTER,"%s", alternatives[quest.first_answer_loc+1]);
						al_draw_textf(fontP, al_map_rgb(255,255,255), WIDTH / 2 - 40, 600, ALLEGRO_ALIGN_CENTER,"%s", alternatives[quest.first_answer_loc+2]);
                    	al_draw_textf(fontM, al_map_rgb(255, 255, 255), WIDTH/2, 20, ALLEGRO_ALIGN_CENTER, "Você pode pedir ajuda para os universitários %i vezes", player.lives);
						al_draw_textf(fontG, al_map_rgb(255, 255, 255), 599, 373, 0, "%i,0", player.score);
						al_draw_textf(fontM, al_map_rgb(255, 255, 255), 600, 440, 0, "Nota");
						al_draw_textf(fontM, al_map_rgb(255, 255, 255), 575, 540, 0, "Sair (ESC)");
						al_draw_textf(fontM, al_map_rgb(255, 255, 255), 575, 670, 0, "Ajuda (H)");
						if(NeedHelp) {
							//550, 440, 330
							al_draw_textf(fontP, al_map_rgb(255,255,255), WIDTH / 2 - 40, tipsID[quest.question_loc], ALLEGRO_ALIGN_CENTER,"Dica: %s", tips[quest.question_loc]);
						}
					}
            	}
            	else if (state == GAMEOVER) { //Caso o jogador responsa as 10 perguntas e acerte menos de 6
                	isGameOver = true;
					FirstTime = true;
					al_draw_bitmap(end,0,0,0); //imagem de fundo
					if (player.ID == STUDENT) {
						al_draw_text(fontG, al_map_rgb(255,0,0), WIDTH / 2, 60, ALLEGRO_ALIGN_CENTER, "VOCÊ REPETIU DE SEMESTRE");
						al_draw_text(fontG, al_map_rgb(255,255,255), WIDTH / 2, 150, ALLEGRO_ALIGN_CENTER, "Menos truco da próxima vez!");
						al_draw_text(fontM, al_map_rgb(255,255,255), WIDTH / 2, 220, ALLEGRO_ALIGN_CENTER, "Pelo menos é uma chance de ficar");
						al_draw_text(fontM, al_map_rgb(255,255,255), WIDTH / 2, 260, ALLEGRO_ALIGN_CENTER, "mais próximo dos mesmos professores :)");
					}
					if (player.ID == PROFESSOR) {
						al_draw_text(fontG, al_map_rgb(255,0,0), WIDTH / 2, 60, ALLEGRO_ALIGN_CENTER, "VOCÊ TERMINOU O SEMESTRE SEM HONRA");
						al_draw_text(fontG, al_map_rgb(255,255,255), WIDTH / 2, 150, ALLEGRO_ALIGN_CENTER, "Menos pausas para o café da próxima vez!");
						al_draw_text(fontM, al_map_rgb(255,255,255), WIDTH / 2, 220, ALLEGRO_ALIGN_CENTER, "No último conselho de classe, você teve as piores notas");
						al_draw_text(fontM, al_map_rgb(255,255,255), WIDTH / 2, 260, ALLEGRO_ALIGN_CENTER, "e nenhum aluno lhe chamou pro barzinho na Hercílio Luz");
					}
					al_draw_textf(fontG, al_map_rgb(255,255,255), WIDTH / 2, 370, ALLEGRO_ALIGN_CENTER,"Você conseguiu %d pontos", player.score);
                	al_draw_text(fontM, al_map_rgb(255,255,255), WIDTH/2, 473, ALLEGRO_ALIGN_CENTER, "ESC PARA SAIR DO JOGO");
                	al_draw_text(fontM, al_map_rgb(255,255,255), WIDTH/2, 600, ALLEGRO_ALIGN_CENTER, "SPACE PARA JOGAR NOVAMENTE");
            	}
				else if (state == WON) {
					isGameOver = true;
					FirstTime = true;
					al_draw_bitmap(end,0,0,0); //imagem de fundo
					if (player.score >= 6 & player.score != 10) { //Caso o jogador responsa as 10 perguntas, acerte mais de 6 e menos de 10
						if (player.ID == STUDENT) {
							al_draw_text(fontG, al_map_rgb(0,255,0), WIDTH / 2, 60, ALLEGRO_ALIGN_CENTER, "VOCÊ CONSEGUIU PASSAR DE SEMESTRE");
							al_draw_text(fontG, al_map_rgb(255,255,255), WIDTH / 2, 150, ALLEGRO_ALIGN_CENTER, "Mais foco da próxima vez!");
							al_draw_text(fontM, al_map_rgb(255,255,255), WIDTH / 2, 220, ALLEGRO_ALIGN_CENTER, "podia ir melhor mas passar é o que importa");
						}
						if (player.ID == PROFESSOR) {
							al_draw_text(fontG, al_map_rgb(0,255,0), WIDTH / 2, 60, ALLEGRO_ALIGN_CENTER, "VOCÊ É UM PROFESSOR MEDIANO");
							al_draw_text(fontG, al_map_rgb(255,255,255), WIDTH / 2, 150, ALLEGRO_ALIGN_CENTER, "Menos pausas para chocolate quente do Vics!");
							al_draw_text(fontM, al_map_rgb(255,255,255), WIDTH / 2, 220, ALLEGRO_ALIGN_CENTER, "Você consegue se explicar mas os alunos não te amam");
							al_draw_text(fontM, al_map_rgb(255,255,255), WIDTH / 2, 260, ALLEGRO_ALIGN_CENTER, "falta mais didática e empatia");
						}
						al_draw_textf(fontG, al_map_rgb(255,255,255), WIDTH / 2, 370, ALLEGRO_ALIGN_CENTER,"Você conseguiu %d pontos", player.score);
                		al_draw_text(fontM, al_map_rgb(255,255,255), WIDTH/2, 473, ALLEGRO_ALIGN_CENTER, "ESC PARA SAIR DO JOGO");
                		al_draw_text(fontM, al_map_rgb(255,255,255), WIDTH/2, 600, ALLEGRO_ALIGN_CENTER, "SPACE PARA JOGAR NOVAMENTE");
					}
					else if (player.score == 10) {
						if (player.ID == STUDENT) {
							al_draw_text(fontG, al_map_rgb(0,255,0), WIDTH / 2, 60, ALLEGRO_ALIGN_CENTER, "ALUNO EXEMPLAR DA SUA TURMA");
							al_draw_text(fontG, al_map_rgb(255,255,255), WIDTH / 2, 150, ALLEGRO_ALIGN_CENTER, "Continue assim!");
							al_draw_text(fontM, al_map_rgb(255,255,255), WIDTH / 2, 220, ALLEGRO_ALIGN_CENTER, "eles vão escrever cartas de recomendação pra você");
							al_draw_text(fontM, al_map_rgb(255,255,255), WIDTH / 2, 260, ALLEGRO_ALIGN_CENTER, "e citar seus trabalhos pra seus próximos alunos");

						}
						if (player.ID == PROFESSOR) {
							al_draw_text(fontG, al_map_rgb(0,255,0), WIDTH / 2, 60, ALLEGRO_ALIGN_CENTER, "MELHOR PROFESSOR DO IFSC");
							al_draw_text(fontG, al_map_rgb(255,255,255), WIDTH / 2, 150, ALLEGRO_ALIGN_CENTER, "Os alunos falam bem de você para os outros");
							al_draw_text(fontM, al_map_rgb(255,255,255), WIDTH / 2, 220, ALLEGRO_ALIGN_CENTER, "Você destruiu na avaliação do conselho e os outros professores");
							al_draw_text(fontM, al_map_rgb(255,255,255), WIDTH / 2, 260, ALLEGRO_ALIGN_CENTER, "estão de queixo no chão tentando descobrir como você consegue");
						}
						al_draw_textf(fontG, al_map_rgb(255,255,255), WIDTH / 2, 370, ALLEGRO_ALIGN_CENTER,"Você gabaritou!");
                		al_draw_text(fontM, al_map_rgb(255,255,255), WIDTH/2, 473, ALLEGRO_ALIGN_CENTER, "ESC PARA SAIR DO JOGO");
                		al_draw_text(fontM, al_map_rgb(255,255,255), WIDTH/2, 600, ALLEGRO_ALIGN_CENTER, "SPACE PARA JOGAR NOVAMENTE");
					}
            	}

            al_flip_display();
            al_clear_to_color(al_map_rgb(0,0,0));
        	}
		}
	}
	//colocar todos
	al_destroy_bitmap(menuimage);
	al_destroy_bitmap(charactermenu);
	al_destroy_bitmap(thematicmenu);
	al_destroy_event_queue(event_queue);
	al_destroy_timer(timer);
	al_destroy_font(fontP);
	al_destroy_font(fontM);
	al_destroy_font(fontG);
	al_destroy_display(display);						//destroy our display object

	return 0;
}

void Character(struct Character *player) { //inicia o personagem
	player->x = 30;
	player->y = 175;
	player->lives = 3; //quantas vezes pode pedir ajuda
	player->score = 0; //começa o jogo com 0 pontos
}

void Interviewer(struct Extras *interviewer, struct Question *quest) {
	if (quest->question_loc < 30) {
		interviewer->ID = INTERVIEWER_PAMELA;
		interviewer->x = 550;
		interviewer->y = 175;
	}
	else {
		interviewer->ID = INTERVIEWER_VALTER;
		interviewer->x = 550;
		interviewer->y = 175;
	}
}

void NewQuestion(struct Question *quest, int questID[60], int questIDans[180]) {
	if (quest->thematic == 1) { //Caso o jogador tenha escolhido jogar com a temática 1
		quest->question_ID = 1000 + (quest->num * 100) + (rand() % 3 + 1);
	}
	else if (quest->thematic == 2) {
		quest->question_ID = 2000 + (quest->num * 100) + (rand() % 3 + 1);
	}
	else if (quest->thematic == 3) {
		quest->question_ID = (rand() % 2 + 1) * 1000 + (quest->num * 100) + (rand() % 3 + 1);
	}

	int j =0;
	while(questID[j]!=quest->question_ID) { //Acrescimo na variável de controle até que a mesma carregue o valor correspondente a localização da primeira resposta da pergunta sorteada
		j++;
	}
	quest->question_loc = j;

	int i = 0; //Cria uma variável de controle
	while(questIDans[i]!=quest->question_ID) { //Acrescimo na variável de controle até que a mesma carregue o valor correspondente a localização da primeira resposta da pergunta sorteada
		i++; 
	}
	quest->first_answer_loc = i; //FirstAnswerLOC contém o valor correspondente a localização da primeira opção de resposta da questão sorteada
}

void Answer(struct Character *player, struct Question *quest, int verifyID[180], ALLEGRO_SAMPLE *right_answer, ALLEGRO_SAMPLE *wrong_answer) {
	int j=quest->first_answer_loc; //Cria uma variáve de controle que carrega o valor da primeira opção de resposta da questão sorteada
	while(verifyID[j]!=1) { //Acrescimo na variável de controle até que ela contenha o valor correspondente a localização da resposta correta
		j++;
	}
	quest->real_answer = j; //answer contém o valor correspondente a localização da resposta correta da questão sorteada
	quest->player_answer = quest->player_answer + quest->first_answer_loc; //Armazena o valor da resposta do usuário (0, 1 ou 2) com o acrescimo da localização da primeira resposta da questão sorteada, ou seja, player_answer contém o valor correspondente a localização da resposta escolhida pelo mesmo
	if (quest->player_answer == quest->real_answer) { //Se a localização da resposta correta for a mesma da resposta do usuário, ele recebe um ponto
		player->score++;
		al_play_sample(right_answer,5,0,1,ALLEGRO_PLAYMODE_ONCE, NULL); //Toca o audio do Silvio Santos comemorando o acerto
	}
	else {
		al_play_sample(wrong_answer,1,0,1,ALLEGRO_PLAYMODE_ONCE, NULL); ////Toca o audio do Silvio Santos comentando o erro
	}
}