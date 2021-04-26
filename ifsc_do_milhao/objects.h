//Object IDS
enum IDS{STUDENT, PROFESSOR, INTERVIEWER_VALTER, INTERVIEWER_PAMELA};

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
	int thematic;
	int question_ID;
	int question_loc;
	int num;
	int first_answer_loc;
	int real_answer;
	int player_answer;
};