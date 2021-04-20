//Object IDS
enum IDS{STUDENT, PROFESSOR, PROFESSOR_X, PROFESSOR_Y, INTERVIEWER_VALTER, INTERVIEWER_PAMELA};

//Our Player
struct Character {
	int ID;
	int x;
	int y;
	int lives;
	int score;
};

//Our Interviewer and Professor
struct Extras {
	int ID;
	int x;
	int y;
};

//Our Questions
struct Question {
	int ID;
	int thematic;
	int num;
	int answer;
	int player_answer;
};