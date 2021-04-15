//Object IDS
enum IDS{PLAYER, INTERVIEWER_VALTER, INTERVIEWER_PAMELA};

//Our Player
struct Character {
	int ID;
	int x;
	int y;
	int lives;
	int score;
};

struct InterviewerValter {
	int ID;
	int x;
	int y;
};

struct InterviewerPamela {
	int ID;
	int x;
	int y;
};