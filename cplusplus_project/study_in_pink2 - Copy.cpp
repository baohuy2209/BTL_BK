#include "study_in_pink2.h"

////////////////////////////////////////////////////////////////////////
/// STUDENT'S ANSWER BEGINS HERE
/// Complete the following functions
/// DO NOT modify any parameters in the functions.
////////////////////////////////////////////////////////////////////////
// Fuction Definition for MapElement
MapElement::MapElement(ElementType in_type) : type(in_type) {}
MapElement::~MapElement() {}
ElementType MapElement::getType() const { return type; }
Path::Path() : MapElement(PATH) {}
Wall::Wall() : MapElement(WALL) {}
FakeWall::FakeWall(int in_req_exp) : MapElement(FAKE_WALL), req_exp(in_req_exp) {}
int FakeWall::getReqExp() const { return req_exp; }
////////////////////////////////////////////////////////////////////////
// Map
////////////////////////////////////////////////////////////////////////
Map::Map(int num_rows, int num_cols, int num_walls, Position *array_walls, int num_fake_walls, Position *array_fake_walls) : num_rows(num_rows), num_cols(num_cols)
{
    // Initialize map with paths for all cells
    map = new MapElement **[num_rows];
    for (int i = 0; i < num_rows; ++i)
    {
        map[i] = new MapElement *[num_cols];
        for (int j = 0; j < num_cols; ++j)
        {
            map[i][j] = new Path();
        }
    }
    for (int i = 0; i < num_fake_walls; ++i)
    {
        int r = array_fake_walls[i].getRow();
        int c = array_fake_walls[i].getCol();
        int in_req_exp = (r * 257 + c * 139 + 89) % 900 + 1;
        delete map[r][c];
        map[r][c] = new FakeWall(in_req_exp);
    }
    for (int i = 0; i < num_walls; ++i)
    {

        int r = array_walls[i].getRow();
        int c = array_walls[i].getCol();
        delete map[r][c];
        map[r][c] = new Wall();
    }
}
Map::~Map()
{
    for (int i = 0; i < num_rows; ++i)
    {
        for (int j = 0; j < num_cols; ++j)
        {
            delete map[i][j];
        }
        delete[] map[i];
    }
    delete[] map;
}
bool Map::isValid(const Position &pos, MovingObject *mv_obj) const
{
    int rowtocheck = pos.getRow();
    int coltocheck = pos.getCol();
    if (rowtocheck < 0 || coltocheck < 0 || rowtocheck >= num_rows || coltocheck >= num_cols)
    {
        return false;
    }
    if (map[rowtocheck][coltocheck]->getType() == WALL)
    {
        return false;
    }
    else if (map[rowtocheck][coltocheck]->getType() == FAKE_WALL)
    {
        if (mv_obj->getname() == "Watson")
        {
            Watson *watson_obj = dynamic_cast<Watson *>(mv_obj);
            FakeWall *fakewall_obj = dynamic_cast<FakeWall *>(map[rowtocheck][coltocheck]);
            int watson_exp = watson_obj->getExp();
            int fakewall_exp = fakewall_obj->getReqExp();
            if (!(watson_exp >= fakewall_exp))
            {
                return false;
            }
        }
    }
    return true;
}

////////////////////////////////////////////////////////////////////////
// Position
////////////////////////////////////////////////////////////////////////
Position::Position(int r, int c) : r(r), c(c) {}
Position::Position(const string &str_pos)
{
    char readchar;
    istringstream iss(str_pos);
    iss >> readchar >> r;
    iss >> readchar >> c;
}
int Position::getRow() const { return r; }
int Position::getCol() const { return c; }
void Position::setRow(int row) { this->r = row; }
void Position::setCol(int col) { this->c = col; }

string Position::str() const
{
    return "(" + to_string(r) + "," + to_string(c) + ")";
}
bool Position::isEqual(int in_r, int in_c) const
{
    return (this->r == in_r && this->c == in_c);
}
bool Position::isEqual(Position pos) const
{
    return (this->r == pos.getRow() && this->c == pos.getCol());
}
const Position Position::npos(-1, -1);
////////////////////////////////////////////////////////////////////////
// Moving Object
////////////////////////////////////////////////////////////////////////
MovingObject::MovingObject(int index, const Position pos, Map *map, const string &name) : index(index), pos(pos), map(map), name(name)
{
}
MovingObject::~MovingObject() {}
string MovingObject::getname()
{
    return name;
}
Position MovingObject::getCurrentPosition() const { return pos; }
////////////////////////////////////////////////////////////////////////
// Sherlock
////////////////////////////////////////////////////////////////////////
Character::Character(int index, const Position pos, Map *map, string character_type, int hp, int exp) : MovingObject(index, pos, map, character_type), hp(hp), exp(exp) {}
int Character::getExp() { return exp; }
int Character::getHp() { return hp; }
void Character::setExp(int in_exp) { this->exp = in_exp; }
void Character::setHp(int in_hp) { this->hp = in_hp; }
Sherlock::Sherlock(int index, const string &moving_rule, const Position &init_pos, Map *map, int init_hp, int init_exp) : Character(index, init_pos, map, "Sherlock", init_hp, init_exp), moving_rule(moving_rule), move_index(0)
{
    if (hp < 0)
    {
        hp = 0;
    }
    if (hp > 500)
    {
        hp = 500;
    }
    if (exp < 0)
    {
        exp = 0;
    }
    if (exp > 900)
    {
        exp = 900;
    }
}
Position Sherlock::getNextPosition()
{
    if (moving_rule.empty() || move_index < 0 || move_index >= moving_rule.size())
    {
        return Position::npos;
    }

    char move = moving_rule[move_index];
    int nrow = pos.getRow();
    int ncol = pos.getCol();
    if (move == 'U')
    {
        nrow--;
    }
    else if (move == 'D')
    {
        nrow++;
    }
    else if (move == 'R')
    {
        ncol++;
    }
    else if (move == 'L')
    {
        ncol--;
    }
    Position new_pos(nrow, ncol);
    if (map->isValid(new_pos, this))
    {
        return new_pos;
    }
    else
    {
        return Position::npos;
    }
}

string Sherlock::str() const
{
    return "Sherlock[index=" + to_string(index) + ";pos=" + pos.str() + ";moving_rule=" + moving_rule + "]";
}
void Sherlock::move()
{
    Position nepos = getNextPosition();
    move_index++;
    if (!nepos.isEqual(-1, -1) && hp != 0)
    {
        this->pos = nepos;
    }
    if (move_index > moving_rule.size() - 1)
    {
        move_index = 0;
    }
}
////////////////////////////////////////////////////////////////////////
// Watson
////////////////////////////////////////////////////////////////////////
Watson::Watson(int index, const string &moving_rule, const Position &init_pos, Map *map, int init_hp, int init_exp) : Character(index, init_pos, map, "Watson", init_hp, init_exp), moving_rule(moving_rule), move_index(0)
{
    if (hp < 0)
    {
        hp = 0;
    }
    if (hp > 500)
    {
        hp = 500;
    }
    if (exp < 0)
    {
        exp = 0;
    }
    if (exp > 900)
    {
        exp = 900;
    }
}
Position Watson::getNextPosition()
{
    if (moving_rule.empty() || move_index < 0 || move_index >= moving_rule.size())
    {
        return Position::npos;
    }
    char move = moving_rule[move_index];
    int nrow = pos.getRow();
    int ncol = pos.getCol();
    switch (move)
    {
    case 'U':
        nrow--;
        break;
    case 'D':
        nrow++;
        break;
    case 'R':
        ncol++;
        break;
    case 'L':
        ncol--;
        break;
    }
    Position new_pos(nrow, ncol);
    if (map->isValid(new_pos, this))
    {
        return new_pos;
    }
    else
    {
        return Position::npos;
    }
}
void Watson::move()
{
    Position nepos = getNextPosition();
    move_index++;
    if (!nepos.isEqual(-1, -1) && hp != 0)
    {
        this->pos = nepos;
    }
    if (move_index > moving_rule.size() - 1)
    {
        move_index = 0;
    }
}
string Watson::str() const
{
    return "Watson[index=" + to_string(index) + ";pos=" + pos.str() + ";moving_rule=" + moving_rule + "]";
}
////////////////////////////////////////////////////////////////////////
// Criminal
////////////////////////////////////////////////////////////////////////
Criminal::Criminal(int index, const Position &init_pos, Map *map, Sherlock *sherlock, Watson *watson) : Character(index, init_pos, map, "Criminal", 0, 0), sherlock(sherlock), watson(watson) {}
Position Criminal::getNextPosition()
{
    Position spos = sherlock->getCurrentPosition();
    Position wpos = watson->getCurrentPosition();

    int rc = pos.getRow();
    int colc = pos.getCol();
    int dist_up = (map->isValid(Position(rc - 1, colc), this)) ? abs((rc - 1) - spos.getRow()) + abs(colc - spos.getCol()) + abs((rc - 1) - wpos.getRow()) + abs(colc - wpos.getCol()) : -1;
    int dist_left = (map->isValid(Position(rc, colc - 1), this)) ? abs(rc - spos.getRow()) + abs((colc - 1) - spos.getCol()) + abs(rc - wpos.getRow()) + abs((colc - 1) - wpos.getCol()) : -1;
    int dist_down = (map->isValid(Position(rc + 1, colc), this)) ? abs((rc + 1) - spos.getRow()) + abs(colc - spos.getCol()) + abs((rc + 1) - wpos.getRow()) + abs(colc - wpos.getCol()) : -1;
    int dist_right = (map->isValid(Position(rc, colc + 1), this)) ? abs(rc - spos.getRow()) + abs((colc + 1) - spos.getCol()) + abs(rc - wpos.getRow()) + abs((colc + 1) - wpos.getCol()) : -1;
    int max_distance = max(max(dist_up, dist_left), max(dist_down, dist_right));
    if (max_distance == -1)
    {
        return Position::npos;
    }
    if (max_distance == dist_up)
    {
        return Position(rc - 1, colc);
    }
    else if (max_distance == dist_left)
    {
        return Position(rc, colc - 1);
    }
    else if (max_distance == dist_down)
    {
        return Position(rc + 1, colc);
    }
    else if (max_distance == dist_right)
    {
        return Position(rc, colc + 1);
    }

    return Position::npos;
}

void Criminal::move()
{
    Position nepos = getNextPosition();
    if (!nepos.isEqual(Position::npos.getRow(), Position::npos.getCol()))
    {
        prevpos = this->pos;
        this->pos = nepos;
    }
}
Position Criminal::get_prevpos()
{
    return prevpos;
}
string Criminal::str() const
{
    return "Criminal[index=" + to_string(index) + ";pos=" + pos.str() + "]";
}
////////////////////////////////////////////////////////////////////////
// Array Moving Objects
////////////////////////////////////////////////////////////////////////
ArrayMovingObject::ArrayMovingObject(int capacity) : capacity(capacity), count(0)
{
    arr_mv_objs = new MovingObject *[capacity];
}
ArrayMovingObject::~ArrayMovingObject()
{
    delete[] arr_mv_objs;
}
bool ArrayMovingObject::isFull() const
{
    return count == capacity;
}

bool ArrayMovingObject::add(MovingObject *mv_obj)
{
    if (isFull())
    {
        return false;
    }
    int index = mv_obj->getIndex();
    arr_mv_objs[index] = mv_obj;
    count++;
    return true;
}
MovingObject *ArrayMovingObject::get(int index) const
{
    return arr_mv_objs[index];
}
int ArrayMovingObject::size() const
{
    return count;
}
string ArrayMovingObject::str() const
{
    string str = "ArrayMovingObject[count=" + to_string(count) + ";capacity=" + to_string(capacity) + ";";
    for (int i = 0; i < count; ++i)
    {
        if (i == count - 1)
        {
            str += arr_mv_objs[i]->str();
        }
        else
        {
            str += arr_mv_objs[i]->str() + ";";
        }
    }
    str += "]";
    return str;
}
Configuration::Configuration(const string &filepath)
{
    ifstream ifs(filepath);
    string line;
    if (ifs.is_open())
    {
        while (getline(ifs, line))
        {
            string attribute, value;
            int a = line.find("=");
            attribute = line.substr(0, a);
            value = line.substr(a + 1, line.length() - a - 1);
            if (attribute == "MAP_NUM_ROWS")
            {
                map_num_rows = stoi(value);
            }
            else if (attribute == "MAP_NUM_COLS")
            {
                map_num_cols = stoi(value);
            }
            else if (attribute == "MAX_NUM_MOVING_OBJECTS")
            {
                max_num_moving_objects = stoi(value);
            }
            else if (attribute == "SHERLOCK_MOVING_RULE")
            {
                sherlock_moving_rule = value;
            }
            else if (attribute == "SHERLOCK_INIT_HP")
            {
                sherlock_init_hp = stoi(value);
            }
            else if (attribute == "SHERLOCK_INIT_EXP")
            {
                sherlock_init_exp = stoi(value);
            }
            else if (attribute == "WATSON_MOVING_RULE")
            {
                watson_moving_rule = value;
            }
            else if (attribute == "WATSON_INIT_HP")
            {
                watson_init_hp = stoi(value);
            }
            else if (attribute == "WATSON_INIT_EXP")
            {
                watson_init_exp = stoi(value);
            }
            else if (attribute == "NUM_STEPS")
            {
                num_steps = stoi(value);
            }
            else if (attribute == "ARRAY_WALLS")
            {
                istringstream issArr(value);
                int count = 0;
                for (char c : value)
                {
                    if (c == ',')
                    {
                        count++;
                    }
                }
                num_walls = count;
                arr_walls = new Position[num_walls];
                for (int i = 0; i < num_walls; ++i)
                {
                    char dummy;
                    int x, y;

                    issArr >> dummy >> dummy >> x >> dummy >> y >> dummy;

                    arr_walls[i] = Position(x, y);
                }
            }
            else if (attribute == "ARRAY_FAKE_WALLS")
            {
                istringstream issArr(value);
                int count = 0;
                for (char c : value)
                {
                    if (c == ',')
                    {
                        count++;
                    }
                }
                num_fake_walls = count;
                arr_fake_walls = new Position[num_fake_walls];
                for (int i = 0; i < num_fake_walls; ++i)
                {
                    char dummy;
                    int x, y;
                    issArr >> dummy >> dummy >> x >> dummy >> y >> dummy;
                    arr_fake_walls[i] = Position(x, y);
                }
            }
            else if (attribute == "SHERLOCK_INIT_POS")
            {
                istringstream issPos(value);
                char dummy;
                int x, y;
                issPos >> dummy >> x >> dummy >> y >> dummy;
                sherlock_init_pos = Position(x, y);
            }
            else if (attribute == "WATSON_INIT_POS")
            {
                istringstream issPos(value);
                char dummy;
                int x, y;
                issPos >> dummy >> x >> dummy >> y >> dummy;
                watson_init_pos = Position(x, y);
            }
            else if (attribute == "CRIMINAL_INIT_POS")
            {
                istringstream issPos(value);
                char dummy;
                int x, y;
                issPos >> dummy >> x >> dummy >> y >> dummy;
                criminal_init_pos = Position(x, y);
            }
        }
    }
}

Configuration::~Configuration()
{
    delete[] arr_walls;
    delete[] arr_fake_walls;
}
string Configuration::str() const
{

    string result = "Configuration[\n";
    result += "MAP_NUM_ROWS=" + to_string(map_num_rows) + "\n";
    result += "MAP_NUM_COLS=" + to_string(map_num_cols) + "\n";
    result += "MAX_NUM_MOVING_OBJECTS=" + to_string(max_num_moving_objects) + "\n";
    result += "NUM_WALLS=" + to_string(num_walls) + "\n";
    string arr_walls_value;
    for (int i = 0; i < num_walls; i++)
    {
        arr_walls_value += arr_walls[i].str();
        if (i != num_walls - 1)
        {
            arr_walls_value += ";";
        }
    }
    result += "ARRAY_WALLS=[" + arr_walls_value + "]" + "\n";

    result += "NUM_FAKE_WALLS=" + to_string(num_fake_walls) + "\n";
    string arr_fake_walls_value;
    for (int i = 0; i < num_fake_walls; i++)
    {
        arr_fake_walls_value += arr_fake_walls[i].str();
        if (i != num_fake_walls - 1)
        {
            arr_fake_walls_value += ";";
        }
    }
    result += "ARRAY_FAKE_WALLS=[" + arr_fake_walls_value + "]" + "\n";

    result += "SHERLOCK_MOVING_RULE=" + sherlock_moving_rule + "\n";
    result += "SHERLOCK_INIT_POS=" + sherlock_init_pos.str() + "\n";
    result += "SHERLOCK_INIT_HP=" + to_string(sherlock_init_hp) + "\n";
    result += "SHERLOCK_INIT_EXP=" + to_string(sherlock_init_exp) + "\n";

    result += "WATSON_MOVING_RULE=" + watson_moving_rule + "\n";
    result += "WATSON_INIT_POS=" + watson_init_pos.str() + "\n";
    result += "WATSON_INIT_HP=" + to_string(watson_init_hp) + "\n";
    result += "WATSON_INIT_EXP=" + to_string(watson_init_exp) + "\n";

    result += "CRIMINAL_INIT_POS=" + criminal_init_pos.str() + "\n";
    result += "NUM_STEPS=" + to_string(num_steps) + "\n";
    return result + "]";
}
Robot::Robot(int index, const Position init_pos, Map * map, const string &name, RobotType robot_type) : MovingObject(index, init_pos, map, name){
	this->robot_type = robot_type; 
	int i = this->pos.getRow(); 
	int j = this->pos.getCol();  
	int p = i*j; 
	// tim so chu dao cua p 
	while(p/10 != 0){
		p = p/10 + p%10; 
	}
	// tao gia tri cho item 
	if((0<=p)&&(p<=1)){
		item = new MagicBook(); 
	}else if((2<= p) && (p <= 3)){
		item = new EnergyDrink();
	}else if((4<= p) && (p <= 5)){
		item = new FirstAid();
	}else if((6<= p) && (p <= 7)){
		item = new ExcemptionCard(); 
	}else if((8<= p) && (p <= 9)){
		int t = (11*i + j)%4; 
		switch(t){
			case 0:
				item = new PassingCard("RobotS");
				break; 
			case 1: 
				item = new PassingCard("RobotC");
				break;
			case 2: 
				item = new PassingCard("RobotSW");
				break; 
			case 3: 
				item = new PassingCard("all");
				break; 
		}				/*
			Các vật phẩm này được chứa trong các robot. Điều kiện tạo ra như sau:
			Gọi vị trí tạo ra robot có toạ độ là (i,j) với i là chỉ số hàng, j là chỉ số cột.
			Với p = i ∗ j. Gọi s số chủ đạo của p . Ta định nghĩa số chủ đạo của một số là giá trị tổng
			các chữ số, cho đến khi giá trị tổng đó là số có 1 chữ số. Ta có:
			Bài tập lớn môn Kỹ thuật lập trình - HK 2 năm học 2023 - 2024 Trang 18/25TRƯỜNG ĐẠI HỌC BÁCH KHOA - ĐHQG-HCM
			KHOA KHOA HỌC VÀ KỸ THUẬT MÁY TÍNH
			• Nếu s nằm trong đoạn [0, 1] thì sẽ tạo ra MagicBook
			• Nếu s nằm trong đoạn [2, 3] thì sẽ tạo ra EnergyDrink
			• Nếu s nằm trong đoạn [4, 5] thì sẽ tạo ra FirstAid
			• Nếu s nằm trong đoạn [6,7] thì sẽ tạo ra ExcemptionCard
			• Nếu s nằm trong đoạn [8, 9] thì sẽ tạo ra PassingCard. Đặt t = (i ∗ 11 + j)%4. Thuộc
			tính challenge của PassingCard được khởi tạo như sau:
			– t = 0: challenge = "RobotS"
			– t = 1: challenge = "RobotC"
			– t = 2: challenge = "RobotSW"
			– t = 3: challenge = "all"
		*/
	}
}
string Robot::getName() const {
	string result = ""; 
	if (this->robot_type == RobotType::C){
		result = "RobotC"; 
	}else if (this->robot_type == RobotType::S){
		result = "RobotS"; 
	}else if (this->robot_type == RobotType::W){
		result = "RobotW"; 
	}else if (this->robot_type == RobotType::SW){
		result = "RobotSW"; 
	}
	return result; 
}
Position RobotC::getNextPosition(){
	Position new_pos = this->criminal->get_prevpos(); 
	if(getDistance(this->criminal) >= 2){
		return new_pos; 
	}else{
		return this->criminal->getCurrentPosition(); 
	}
	
}
void RobotC::move(){
	Position result = getNextPosition(); 
	if(!result.isEqual(Position::npos.getRow(), Position::npos.getCol())){
		this->pos = result; 
	}
}
int RobotC::getDistance(MovingObject * moving_object) const{
	int x1 = this->pos.getRow(); 
	int y1 = this->pos.getCol();
	const Position obj_pos = moving_object->getCurrentPosition(); 
	int x2 = obj_pos.getRow(); 
	int y2 = obj_pos.getCol(); 
	
	int result = abs(x1-x2) + abs(y1-y2); 
	return result; 
}
string RobotC::str() const{
	return "Robot[pos="+this->pos.str()+";type=C;dist=" "]"; 
}
Position RobotS::getNextPosition(){
	Position sherlock_pos = sherlock->getCurrentPosition();
	int r = this->pos.getRow(); 
	int c = this->pos.getCol();
	int move_up = (this->map->isValid(Position(r-1,c),this)) ? abs((r - 1) - sherlock_pos.getRow()) + abs(c - sherlock_pos.getCol()) : -1; 
	int move_down = (this->map->isValid(Position(r+1,c),this)) ? abs((r + 1) - sherlock_pos.getRow()) + abs(c - sherlock_pos.getCol()) : -1;
	int move_left = (this->map->isValid(Position(r,c-1),this)) ? abs(r - sherlock_pos.getRow()) + abs((c-1) - sherlock_pos.getCol()) : -1;
	int move_right = (this->map->isValid(Position(r,c+1),this)) ? abs(r - sherlock_pos.getRow()) + abs((c+1) - sherlock_pos.getCol()) : -1;
	
	int valid_distance[4] = {move_up, move_down, move_left, move_right};
	
	int min_distance = 0; 
	for(int i = 0; i < 4; i++){
		if(valid_distance[i] != -1){
			min_distance = min(min_distance, valid_distance[i]); 
		}
	}
	if(min_distance == 0){
		return Position::npos; 
	}else if(min_distance == move_up){
		return Position(r-1,c);
	}else if(min_distance == move_right){
		return Position(r,c+1);
	}else if(min_distance == move_down){
		return Position(r+1,c);
	}else if(min_distance == move_left){
		return Position(r,c-1);
	}
	return Position::npos; 
}
void RobotS::move(){
	Position result = getNextPosition(); 
	if(!result.isEqual(Position::npos.getRow(), Position::npos.getCol())){
		this->pos = result; 
	}
}
int RobotS::getDistance() const{
	int x1 = pos.getRow(); 
	int y1 = pos.getCol();
	Position pos_obj = sherlock->getCurrentPosition(); 
	int x2 = pos_obj.getRow(); 
	int y2 = pos_obj.getCol(); 
	
	return abs(x1-x2) + abs(y1-y2); 
}
string RobotS::str() const{
	int dist = getDistance(); 
	return "Robot[pos="+this->pos.str()+";type=S;dist="+to_string(dist)+"]"; 
}
Position RobotW::getNextPosition(){
	Position watson_pos = watson->getCurrentPosition();
	int r = this->pos.getRow(); 
	int c = this->pos.getCol();
	int move_up = (this->map->isValid(Position(r-1,c),this)) ? abs((r - 1) - watson_pos.getRow()) + abs(c - watson_pos.getCol()) : -1; 
	int move_down = (this->map->isValid(Position(r+1,c),this)) ? abs((r + 1) - watson_pos.getRow()) + abs(c - watson_pos.getCol()) : -1;
	int move_left = (this->map->isValid(Position(r,c-1),this)) ? abs(r - watson_pos.getRow()) + abs((c-1) - watson_pos.getCol()) : -1;
	int move_right = (this->map->isValid(Position(r,c+1),this)) ? abs(r - watson_pos.getRow()) + abs((c+1) - watson_pos.getCol()) : -1;
	
	int valid_distance[4] = {move_up, move_right, move_down, move_left};
	
	int min_distance = 0; 
	for(int i = 0; i < 4; i++){
		if(valid_distance[i] != -1){
			min_distance = min(min_distance, valid_distance[i]); 
		}
	}
	if(min_distance == 0){
		return Position::npos; 
	}else if(min_distance == move_up){
		return Position(r-1,c);
	}else if(min_distance == move_right){
		return Position(r,c+1);
	}else if(min_distance == move_down){
		return Position(r+1,c);
	}else if(min_distance == move_left){
		return Position(r,c-1);
	}
	return Position::npos; 
}
void RobotW::move(){
	Position result = getNextPosition(); 
	if(!result.isEqual(Position::npos.getRow(), Position::npos.getCol())){
		pos = result; 
	}
}
int RobotW::getDistance() const{
	int x1 = pos.getRow(); 
	int y1 = pos.getCol();
	Watson * temp_watson = this->watson; 
	Position pos_obj = temp_watson->getCurrentPosition();
	int x2 = pos_obj.getRow(); 
	int y2 = pos_obj.getCol(); 
	
	return abs(x1-x2) + abs(y1-y2); 
}
string RobotW::str() const{
	const int dist = getDistance(); 
	return "Robot[pos="+this->pos.str()+";type=W;dist="+to_string(dist)+"]"; 
}
Position RobotSW::getNextPosition(){
	Position spos = sherlock->getCurrentPosition();
    Position wpos = watson->getCurrentPosition();
    int rc = pos.getRow();
    int colc = pos.getCol();
    int move_upup = (map->isValid(Position(rc - 2, colc), this)) ? abs((rc - 2) - spos.getRow()) + abs(colc - spos.getCol()) + abs((rc - 2) - wpos.getRow()) + abs(colc - wpos.getCol()) : -1;
    int move_upright = (map->isValid(Position(rc - 1, colc + 1), this)) ? abs((rc - 1) - spos.getRow()) + abs(colc + 1 - spos.getCol()) + abs((rc - 1) - wpos.getRow()) + abs(colc + 1 - wpos.getCol()) : -1;
    int move_rightright = (map->isValid(Position(rc, colc + 2), this)) ? abs(rc - spos.getRow()) + abs(colc + 2 - spos.getCol()) + abs(rc - wpos.getRow()) + abs(colc + 2 - wpos.getCol()) : -1;
    int move_downright = (map->isValid(Position(rc + 1, colc + 1), this)) ? abs(rc + 1 - spos.getRow()) + abs((colc + 1) - spos.getCol()) + abs(rc + 1 - wpos.getRow()) + abs((colc + 1) - wpos.getCol()) : -1;
    int move_downdown = (map->isValid(Position(rc + 2, colc), this)) ? abs((rc + 2) - spos.getRow()) + abs(colc - spos.getCol()) + abs((rc + 2) - wpos.getRow()) + abs(colc - wpos.getCol()) : -1;
    int move_downleft = (map->isValid(Position(rc + 1, colc - 1), this)) ? abs(rc + 1 - spos.getRow()) + abs((colc - 1) - spos.getCol()) + abs(rc + 1 - wpos.getRow()) + abs((colc - 1) - wpos.getCol()) : -1;
    int move_leftleft = (map->isValid(Position(rc, colc - 2), this)) ? abs(rc - spos.getRow()) + abs(colc - 2 - spos.getCol()) + abs(rc - wpos.getRow()) + abs(colc - 2 - wpos.getCol()) : -1;
    int move_upleft = (map->isValid(Position(rc - 1, colc - 1), this)) ? abs((rc - 1) - spos.getRow()) + abs(colc - 1 - spos.getCol()) + abs((rc - 1) - wpos.getRow()) + abs(colc - 1 - wpos.getCol()) : -1;
    int valid_distances[8] = {move_upup, move_upright, move_rightright, move_downright, move_downdown, move_downleft, move_leftleft, move_upleft};
    int min_distance = 0;
    for (int i = 0; i < 8; i++)
    {
        if (valid_distances[i] != -1)
        {
            min_distance = min(min_distance, valid_distances[i]);
        }
    }
    if(min_distance == 0){
		return Position::npos; 
	}
    else if(min_distance == move_upup){
    	return Position(rc - 2, colc); 
	}
    else if(min_distance == move_upright){
    	return Position(rc - 1, colc + 1); 
	} 
    else if (min_distance == move_rightright){
		return Position(rc, colc + 2);
	}
    else if (min_distance == move_downright){
    	return Position(rc + 1, colc + 1);
	}
    else if (min_distance == move_downdown){
    	return Position(rc + 2, colc); 
	}
   	else if (min_distance == move_downleft){
    	return Position(rc + 1, colc - 1);
	}
    else if (min_distance == move_leftleft){
    	return Position(rc, colc - 2);
	}
    else if (min_distance == move_upleft){
    	return Position(rc - 1, colc - 1);
	}
	return Position::npos; 
}
void RobotSW::move(){
	Position result = getNextPosition(); 
	if(!result.isEqual(Position::npos.getRow(), Position::npos.getCol())){
		this->pos = result; 
	}
}
int RobotSW::getDistance() const{
	int x1 = this->pos.getRow(); 
	int y1 = this->pos.getCol();
	
	Watson * temp_watson = this->watson; 
	Position pos_watson = temp_watson->getCurrentPosition();
	int x2 = pos_watson.getRow(); 
	int y2 = pos_watson.getCol(); 
	int dist_watson = abs(x1-x2) + abs(y1-y2);
	
	Sherlock * temp_sherlock = this->sherlock; 
	Position pos_sherlock = temp_sherlock->getCurrentPosition();
	int x3 = pos_sherlock.getRow(); 
	int y3 = pos_sherlock.getCol(); 
	int dist_sherlock = abs(x1-x3) + abs(y1-y3);
	
	return dist_watson + dist_sherlock;
}
string RobotSW::str() const{
	const int dist = getDistance(); 
	return "Robot[pos="+this->pos.str()+";type=SW;dist="+to_string(dist)+"]"; 
}
BaseItem::~BaseItem(){}
bool MagicBook::canUse ( Character * obj , Robot * robot ){
	bool flag = false; 
	if(obj->getExp() <= 350){
		flag = true; 
		return flag; 
	}
	return flag; 
}
void MagicBook::use( Character * obj , Robot * robot ){
	int current_exp = obj->getExp(); 
	obj->setExp(current_exp + int(current_exp*0.25)); 
}
bool FirstAid::canUse ( Character * obj , Robot * robot ){
	if((obj->getHp() <= 100) || (obj->getExp() <= 350)) {
		return true; 
	}
	return false; 
}
void FirstAid::use( Character * obj , Robot * robot ){
	int current_hp = obj->getHp(); 
	obj->setHp(current_hp + int(current_hp*0.5));
}
bool ExcemptionCard::canUse ( Character * obj , Robot * robot ){
	if((obj->getname() == "Sherlock") && (obj->getHp()%2 == 1)){
		return true; 
	}
	return false; 
}
void ExcemptionCard::use( Character * obj , Robot * robot ){

}
bool EnergyDrink::canUse ( Character * obj , Robot * robot ){
	if(obj->getHp() <= 100){
		return true; 
	}
	return false; 
}
void EnergyDrink::use( Character * obj , Robot * robot ){
	int current_hp = obj->getHp(); 
	obj->setHp(current_hp + int(current_hp*0.2)); 
}
bool PassingCard::canUse ( Character * obj , Robot * robot ){
	if(obj->getname() == "Watson" && (obj->getExp()%2 == 0)){
		return true; 
	}
	return false; 
}
/*
	Khi sử dụng thẻ PassingCard thực hiện thử thách giúp nhân vật không cần thực hiện thử thách 
	tại một vị trí đi đến. Thẻ có một thuộc tính là challenge là tên của một thử thách 

	(ví dụ loại RobotS là thẻ để vượt qua thử thách của RobotS đặt ra mà không cần thực hiện). 
	Nếu loại của thẻ là all thẻ có thể sử dụng cho bất kì loại nào mà không cần quan tâm thử thách gặp phải là gì. 
	Nếu không, khi sử dụng phương thức use để thực hiện hành động tác động lên sherlock và watson hay không thì cần 
	phải kiểm tra loại thẻ trùng với loại hành động thử thách mà nhân vật gặp phải. Trong trường hợp này, nếu không trùng 
	thể loại thẻ thì sẽ khiến exp của nhân vật bị trừ đi 50 EXP mặc dù vẫn thực hiện được. 
*/
void PassingCard::use( Character * obj , Robot * robot ){
	if(challenge != robot->getName() && challenge != "all"){
		obj->setExp(obj->getExp() - 50); 
	}
}
BaseBag::~BaseBag(){}
int SherlockBag::countItem() const{
	int count = 0; 
	Node* tmp = this->bag; 
	while(tmp != NULL){
		count++; 
		tmp = tmp->getNext(); 
	}
	return count; 
}
// them do vao tui 
bool SherlockBag::insert(BaseItem * item){
	bool flag = true; 
	Node* newNode = new Node(item); 
	int num_current_item = countItem();
	if(num_current_item < this->capacity){
		if(this->bag == NULL){
			this->bag = newNode;
		}else{
			newNode->next = this->bag; 
			this->bag = newNode;
		} 
		return flag; 
	}else{
		flag = false; 
		return flag; 
	}
}
BaseItem* SherlockBag::get(){
	if(bag == NULL){
		return NULL; 
	}
	Node* currentnode = this->bag; 
	Node* previousnode; 
	while(currentnode != NULL){
		BaseItem * current_item = currentnode->item; 
		if(current_item->canUse(obj, NULL)){
			break; 
		}
		previousnode = currentnode; 
		currentnode = currentnode->next;
	}
	if(currentnode == NULL){
		return NULL; 
	}
	previousnode->next = bag;
	bag->next = currentnode->next;  
	currentnode->next= bag->next; 
	bag = currentnode; 
	
	BaseItem* first_item = bag->getItem(); 
	bag = bag->next; 
	
	return first_item;
}	
BaseItem* SherlockBag::get(ItemType itemType){
	if(bag == NULL){
		return NULL; 
	}
	Node* currentnode = bag; 
	Node* previousnode = NULL; 
	while(currentnode != NULL){
		
		if((currentnode->item->getItemType() == itemType) && (currentnode->item->canUse(obj, NULL))){
			break; 
		}
		previousnode = currentnode; 
		currentnode = currentnode->next;
	}
	if(currentnode == NULL){
		return NULL; 
	}
	previousnode->next = bag;
	bag->next = currentnode->next;  
	currentnode->next= bag->next; 
	bag = currentnode; 
	
	BaseItem* first_item = bag->getItem(); 
	bag = bag->next; 
	
	return first_item; 
}
string SherlockBag::str() const{
	int count = countItem(); 
	string str_count = convert_int_to_string(count); 
	string result="Bag[count="+str_count; 
	Node* currentnode = this->bag; 
	while(currentnode != NULL){
		ItemType type = currentnode->item->getItemType();
		switch(type){
			case MAGIC_BOOK: 
				result += ",MagicBook"; 
				break; 
			case ENERGY_DRINK: 
				result += ",EnergyDrink"; 
				break; 
			case FIRST_AID: 
				result += ",FirstAid"; 
				break; 
			case EXCEMPTION_CARD: 
				result += ",ExcemptionCard"; 
				break; 
			case PASSING_CARD: 
				result += ",PassingCard"; 
				break; 
		}
		currentnode = currentnode->next; 
	}
	result += "]"; 
	return result; 
}
int WatsonBag::countItem() const{
	int count = 0; 
	Node* tmp = this->bag; 
	while(tmp != NULL){
		count++; 
		tmp = tmp->getNext(); 
	}
	return count; 
}
bool WatsonBag::insert(BaseItem * item){
	bool flag = true; 
	Node* newNode = new Node(item); 
	int num_current_item = countItem();
	if(num_current_item < this->capacity){
		if(this->bag == NULL){
			this->bag = newNode;
		}else{
			newNode->next = this->bag; 
			this->bag = newNode;
		} 
		return flag; 
	}else{
		flag = false; 
		return flag; 
	}
}
BaseItem* WatsonBag::get(){
	if(bag == NULL){
		return NULL; 
	}
	Node* currentnode = this->bag; 
	Node* previousnode; 
	while(currentnode != NULL){
		BaseItem * current_item = currentnode->item; 
		if(current_item->canUse(obj, NULL)){
			break; 
		}
		previousnode = currentnode; 
		currentnode = currentnode->next;
	}
	if(currentnode == NULL){
		return NULL; 
	}
	previousnode->next = bag;
	bag->next = currentnode->next;  
	currentnode->next= bag->next; 
	bag = currentnode; 
	
	BaseItem* first_item = bag->getItem(); 
	bag = bag->next; 
	
	return first_item;
}
BaseItem* WatsonBag::get(ItemType itemType){
	if (bag == NULL){
		return NULL;
	}
	Node* currentnode = bag; 
	Node* previousnode = NULL; 
	while(currentnode != NULL){
		
		if((currentnode->item->getItemType() == itemType) && (currentnode->item->canUse(obj, NULL))){
			break; 
		}
		previousnode = currentnode; 
		currentnode = currentnode->next;
	}
	if(currentnode == NULL){
		return NULL;
	}
	previousnode->next = bag;
	bag->next = currentnode->next;  
	currentnode->next= bag->next; 
	bag = currentnode; 
	
	BaseItem* first_item = bag->getItem(); 
	bag = bag->next; 
	
	return first_item; 
}
string WatsonBag::str() const{
	int count = this->countItem(); 
	string str_count = convert_int_to_string(count); 
	string result="Bag[count="+str_count; 
	Node* currentnode = this->bag; 
	while(currentnode != NULL){
		ItemType type = currentnode->item->getItemType();
		switch(type){
			case MAGIC_BOOK: 
				result += ",MagicBook"; 
				break; 
			case ENERGY_DRINK: 
				result += ",EnergyDrink";
				break; 
			case FIRST_AID: 
				result += ",FirstAid"; 
				break; 
			case EXCEMPTION_CARD: 
				result += ",ExcemptionCard"; 
				break; 
			case PASSING_CARD: 
				result += ",PassingCard"; 
				break; 
		}
		currentnode = currentnode->next; 
	}
	result += "]"; 
	return result; 
}
/*
	Trong trường hợp Sherlock và Watson gặp nhau, Sherlock tặng Watson thẻ passingCard nếu có 
	và ngược lại Watson sẽ tặng thẻ Excemption Card nếu có. Nếu có nhiều hơn một thẻ trong túi đố 
	, họ sẽ tặng đối phương tất cả. Hành động tặng tức là việc xóa các thẻ ấy ở túi đồ của mình và
	thêm vào đầu của túi đồ người kia. Hành động trao đổi này sẽ diễn ra theo thứ tự Sherlock trước 
	rồi mới đến Watson. Trong trường hợp một trong hai không có hoặc cả hai đều không có loại vật phẩm 
	ấy thì hành động trao đổi không diễn ra.
*/
/*
	 Thứ tự nhân vật tặng vật phẩm là Sherlock tặng Watson rồi mới đến Watson tặng sherlock
*/
/*
	Thứ tự việc tặng vật phẩm của một nhân vật là nhân vật sẽ tìm từ đầu đến cuối túi thông qua phương thức get.
	Mỗi lần tìm thấy 1 vật phẩm có thể tặng được, nhân vật sẽ xóa vật phẩm ra khỏi túi của mình và thêm 
*/
StudyPinkProgram::StudyPinkProgram(const string &config_file_path){
	this->config = new Configuration(config_file_path); 
	this->map = new Map(config->map_num_rows, config->map_num_cols, config->num_walls, config->arr_walls, config->num_fake_walls, config->arr_fake_walls); 
	this->sherlock = new Sherlock(1, config->sherlock_moving_rule, config->sherlock_init_pos, map, config->sherlock_init_hp, config->sherlock_init_exp); // index của sherlock là 1 
	this->watson = new Watson(2, config->watson_moving_rule, config->watson_init_pos, map, config->watson_init_hp, config->watson_init_exp);  // index của watson là 2 
	this->criminal = new Criminal(0, config->criminal_init_pos, map, sherlock, watson); // index cua criminal là 0 
	this->arr_mv_objs = new ArrayMovingObject(config->max_num_moving_objects); 
	this->watsonbag = new WatsonBag(watson); 
	this->sherlockbag = new SherlockBag(sherlock); 
	this->robot = nullptr; 
	// thêm các đói tượng mới khởi tạo vào trong arr ngay lập túc
	arr_mv_objs->add(criminal);  // tuong 
	arr_mv_objs->add(sherlock); 
	arr_mv_objs->add(watson); 
}
bool StudyPinkProgram::isStop() const {
	Position criminal = this->criminal->getCurrentPosition(); 
	Position sherlock_position = this->sherlock->getCurrentPosition();
	Position watson_position = this->watson->getCurrentPosition();
	if(sherlock_position.isEqual(criminal.getRow(), criminal.getCol())){
		return true; 
	}else if(watson_position.isEqual(criminal.getRow(), criminal.getCol())){
		return true; 
	}else if(sherlock->getHp() == 0 || watson->getHp() == 0){
		return true; 
	}
	return false; 
}

void StudyPinkProgram::sherlockcheckmeet(bool verbose, int current_index, int goal_index){
	
/*
	Mỗi khi Sherlock hoặc Watson gặp một loại Robot nào đó, mỗi người sẽ có 2 thời điểm tìm
	kiếm Vật phẩm trong Túi đồ (sẽ mô tả ở Mục sau). Thời điểm sử dụng đầu tiên là khi vừa
	gặp Robot, họ sẽ tìm thử trong túi đồ có vật phầm nào giúp vượt qua thử thách của Robot
	hay không. Khi đó, tham số robot cần truyền vào đối tượng Robot. Thời điểm đầu chỉ có thể
	sử dụng ExcemptionCard hoặc PassingCard. Thời điểm sử dụng thứ 2 là ở cuối lúc gặp
	Robot (sau khi thực hiện thử thách nếu thử thách có xảy ra), Sherlock hoặc Watson sẽ lại tìm
	trong túi đồ thử có vật phẩm nào có thể hồi phục được hp hoặc exp của họ hay không. Khi
	đó, tham số robot có giá trị là NULL. Thời điểm thứ hai chỉ có thể sử dụng MagicBook,
	EnergyDrink hoặc FirstAid
	Các class MagicBook, EnergyDrink, FirstAid, ExcemptionCard, PassingCard lần lượt biểu
	diễn cho các vật phẩm: Nước tăng lực, túi cứu thương hồi phục HP, thẻ miễn trừ ảnh hưởng
	của chướng ngại, thẻ miễn trừ thực hiện thử thách. Riêng đối với thẻ miễn trừ thực hiện thử
	thách có thuộc tính loại thẻ để thể hiện loại thử thách mà thẻ có thể miễn trừ. Các class này
	kế thừa từ class BaseItem và cần phải định nghĩa lại 2 method canUse và use sao cho phù hợp
*/	
	Position current_pos_sherlock = sherlock->getCurrentPosition(); 
	Position current_pos_watson = watson->getCurrentPosition();
	/**
	Trong trường hợp Sherlock và Watson gặp nhau, Sherlock tặng Watson thẻ PassingCard
nếu có và ngược lại Watson sẽ tặng thẻ ExcepmtionCard nếu có. Nếu có nhiều hơn một thẻ
trong túi đồ, họ sẽ tặng cho đối phương tất cả.
	*/
	if(current_pos_sherlock.isEqual(current_pos_watson.getRow(), current_pos_watson.getCol())){
		/*
			 Nếu verbose bằng true thì cần in ra
			thông tin của mỗi MovingObject trong ArrayMovingObject sau khi thực hiện một
			bước di chuyển và các cập nhật sau đó nếu có (ví dụ như Watson gặp một Robot và
			thực hiện thử thách với Robot).
		*/
		if(verbose){
			cout<<"Sherlock meets Watson and exchange their items"<<endl; 
		}
		/*
			Check trong túi của sherlock có passingcard hay không 
			Thứ tự việc tặng
			vật phẩm của một nhân vật là nhân vật sẽ tìm từ đầu đến cuối của túi thông qua phương thức
			get.
		*/
		BaseItem * passingcard = sherlockbag->get(ItemType::PASSING_CARD); 
		while(passingcard != NULL){
			watsonbag->insert(passingcard);
			passingcard = sherlockbag->get(ItemType::PASSING_CARD);
		}
		/*
			Check trong túi của watson có excemptioncard hay không 
		*/
		BaseItem * excemptioncard = watsonbag->get(ItemType::EXCEMPTION_CARD); 
		while(excemptioncard != NULL){
			sherlockbag->insert(excemptioncard);
			excemptioncard = watsonbag->get(ItemType::EXCEMPTION_CARD);  

		}
	}
	for(int i = current_index; i < goal_index; i++){
		Position current_pos_obj =  arr_mv_objs->get(i)->getCurrentPosition(); 
		if(current_pos_sherlock.isEqual(current_pos_obj.getRow(), current_pos_obj.getCol())){
			bool excemption_card = false;
			// lay iem excemption card trong tui 
			// coi nhu da xoa ra khoi tui do 
			BaseItem *current_excemptioncard = sherlockbag->get(ItemType::EXCEMPTION_CARD); 
			if((current_excemptioncard != NULL) &&(current_excemptioncard->canUse(sherlock, robot))){
				excemption_card = true; 
				current_excemptioncard = NULL; 
			}
			MovingObject * current_obj = arr_mv_objs->get(i); 


			if(current_obj->getname() == "RobotS"){
				RobotS *robot = dynamic_cast<RobotS *>(current_obj); 
				if(verbose){
					cout<<"Sherlock meet RobotS"<<endl; 
				}
				if(sherlock->getExp() > 400){
					bool check = sherlockbag->insert(robot->getItem());
					if(check){
						if(verbose){
							cout<<"Sherlock defeat RobotS and get an item from RobotS"<<endl;
						}
					}else{
						if(verbose){
							cout<<"Sherlock defeat RobotS and can't get an item from RobotS because the bag is full"<<endl;
						}
					}
				}else{
					if(!excemption_card){
						// khong vuot qua duoc thu thach nen bi tru 10% mau
						int current_exp_sherlock = sherlock->getExp();
						int exp_sherlock_after = (int) current_exp_sherlock*0.0;
						sherlock->setExp(exp_sherlock_after); 
						if(verbose){
							cout<<"Sherlock don't overcome the challenge. "<<endl; 
						}
					}else{
						// su dung the excemption card de vuot qua thu thach khi khong vuot qua thu thach 
						if(verbose){
							cout<<"Sherlock use Excemption Card. "<<endl; 
						}
					}

				}
			}


			if(current_obj->getname() == "RobotW"){
				RobotW *robot = dynamic_cast<RobotW *>(current_obj); 
				if(verbose){
					cout<<"Sherlock meet RobotW"<<endl; 
				}
				bool check = sherlockbag->insert(robot->getItem());
				if(check){
					if(verbose){
						cout<<"Sherlock defeat RobotW and get an item from RobotW ."<<endl;
					}
				}else{
					if(verbose){
						cout<<"Sherlock defeat RobotW and can't get an item from RobotW because the bag is full ."<<endl;
					}
				}
			}
			if(current_obj->getname() == "RobotSW"){
				RobotSW *robot = dynamic_cast<RobotSW *>(current_obj); 
				if(verbose){
					cout<<"Sherlock meet RobotSW"<<endl; 
				}
				if((sherlock->getExp() > 300)&&(sherlock->getHp()>335)){
					bool check = sherlockbag->insert(robot->getItem()); 
					if(check){
						if(verbose){
							cout<<"Sherlock defeat RobotSW and get an item from RobotSW ."<<endl;
						}
					}else{
						if(verbose){
							cout<<"Sherlock defeat RobotSW and can't get an item from RobotSW because the bag is full ."<<endl; 
						}
					}
				}else{
					// k co item excemption card
					if(!excemption_card){

						int current_exp_sherlock = sherlock->getExp(); 
						int current_hp_sherlock = sherlock->getHp();
						// giam mau va kinh nghiem khi khong vuot qua thu thach 
						int exp_sherlock_after = (int) current_exp_sherlock*0.85; 
						int hp_sherlock_after = (int) current_hp_sherlock*0.85; 
						sherlock->setExp(exp_sherlock_after); 
						sherlock->setHp(hp_sherlock_after); 
					}else{
						if(verbose){
							cout<<"Sherlock using excemption card and overcome the challenge"<<endl; 
						}
					}
				}
			}
			if(current_obj->getname() == "RobotC"){
				RobotC *robot = dynamic_cast<RobotC *>(current_obj); 
				if(verbose){
					cout<<"Sherlock meet RobotC"<<endl; 
				}
				if(sherlock->getExp() > 500){
					Position current_pos_criminal = criminal->getCurrentPosition();
					sherlock->setPosition(current_pos_criminal);
					if(verbose){
						cout<<"Sherlock catch criminal."<<endl; 
					}
				}else{
					bool check = sherlockbag->insert(robot->getItem()); 
					if(check){
						if(verbose){
							cout<<"Sherlock don't catch criminal and get item from RobotC ."<<endl; 
						}	
					}
					else{
						if(verbose){
							cout<<"Sherlock don't catch criminal and can't get item from RobotC because the bag is full."<<endl; 
						}	
					}
				}
			}
			/*
				Thời điểm thứ hai chỉ có thể sử dụng MagicBook,
				EnergyDrink hoặc FirstAid
			*/
			Node* currentbag = sherlockbag->getBag();
			while(currentbag != NULL){

				if((currentbag->item->getItemType() == ItemType::ENERGY_DRINK) && (currentbag->item->canUse(sherlock, NULL))){
					BaseItem* useitem = sherlockbag->get(ItemType::ENERGY_DRINK);
					useitem->use(sherlock, NULL); 
					cout<<"Sherlock use an item"<<endl;
					break; 
				}else if(currentbag->item->getItemType() == ItemType::FIRST_AID){
					BaseItem* useitem = sherlockbag->get(ItemType::FIRST_AID);
					useitem->use(sherlock, NULL); 
					cout<<"Sherlock use an item"<<endl;
					break; 
				}else if(currentbag->item->getItemType() == ItemType::MAGIC_BOOK){
					BaseItem* useitem = sherlockbag->get(ItemType::MAGIC_BOOK);
					useitem->use(sherlock, NULL); 
					cout<<"Sherlock use an item"<<endl;
					break; 
				}
				currentbag = currentbag->getNext(); 
			}

		}
	}
	// Dieu chinh lai luong hp va exp cua Sherlock
	if (sherlock->getHp() < 0)
    {
        sherlock->setHp(0);
    }
    if (sherlock->getExp() < 0)
    {
        sherlock->setExp(0);
    }
    if (sherlock->getHp() > 500)
    {
        sherlock->setHp(500);
    }
    if (sherlock->getExp() > 900)
    {
        sherlock->setExp(900);
    }
}

void StudyPinkProgram::watsoncheckmeet(bool verbose, int current_index, int goal_index){
	/*
	Mỗi khi Sherlock hoặc Watson gặp một loại Robot nào đó, mỗi người sẽ có 2 thời điểm tìm
	kiếm Vật phẩm trong Túi đồ (sẽ mô tả ở Mục sau). Thời điểm sử dụng đầu tiên là khi vừa
	gặp Robot, họ sẽ tìm thử trong túi đồ có vật phầm nào giúp vượt qua thử thách của Robot
	hay không. Khi đó, tham số robot cần truyền vào đối tượng Robot. Thời điểm đầu chỉ có thể
	sử dụng ExcemptionCard hoặc PassingCard. Thời điểm sử dụng thứ 2 là ở cuối lúc gặp
	Robot (sau khi thực hiện thử thách nếu thử thách có xảy ra), Sherlock hoặc Watson sẽ lại tìm
	trong túi đồ thử có vật phẩm nào có thể hồi phục được hp hoặc exp của họ hay không. Khi
	đó, tham số robot có giá trị là NULL. Thời điểm thứ hai chỉ có thể sử dụng MagicBook,
	EnergyDrink hoặc FirstAid
	Các class MagicBook, EnergyDrink, FirstAid, ExcemptionCard, PassingCard lần lượt biểu
	diễn cho các vật phẩm: Nước tăng lực, túi cứu thương hồi phục HP, thẻ miễn trừ ảnh hưởng
	của chướng ngại, thẻ miễn trừ thực hiện thử thách. Riêng đối với thẻ miễn trừ thực hiện thử
	thách có thuộc tính loại thẻ để thể hiện loại thử thách mà thẻ có thể miễn trừ. Các class này
	kế thừa từ class BaseItem và cần phải định nghĩa lại 2 method canUse và use sao cho phù hợp
*/	
	Position current_pos_sherlock = sherlock->getCurrentPosition(); 
	Position current_pos_watson = watson->getCurrentPosition();
	if(current_pos_sherlock.isEqual(current_pos_watson.getRow(), current_pos_watson.getCol())){
		/*
			 Nếu verbose bằng true thì cần in ra
			thông tin của mỗi MovingObject trong ArrayMovingObject sau khi thực hiện một
			bước di chuyển và các cập nhật sau đó nếu có (ví dụ như Watson gặp một Robot và
			thực hiện thử thách với Robot).
		*/
		if(verbose){
			cout<<"Sherlock meets Watson and exchange their items"<<endl; 
		}
		/*
			Check trong túi của sherlock có passingcard hay không 
		*/
		BaseItem * passingcard = sherlockbag->get(ItemType::PASSING_CARD); 
		while(passingcard != NULL){
			watsonbag->insert(passingcard);
			passingcard = sherlockbag->get(ItemType::PASSING_CARD);
		}
		/*
			Check trong túi của watson có excemptioncard hay không 
		*/
		BaseItem * excemptioncard = watsonbag->get(ItemType::EXCEMPTION_CARD); 
		while(excemptioncard != NULL){
			sherlockbag->insert(excemptioncard);
			excemptioncard = watsonbag->get(ItemType::EXCEMPTION_CARD);  

		}
	}
	for(int i = current_index; i < goal_index; i++){
		Position current_pos_obj =  arr_mv_objs->get(i)->getCurrentPosition(); 
		if(current_pos_watson.isEqual(current_pos_obj.getRow(), current_pos_obj.getCol())){
			MovingObject * current_obj = arr_mv_objs->get(i); 

			if(current_obj->getname() == "RobotS"){ 
				if(verbose){
					cout<<"Watson meet RobotS"<<endl; 
				}
			}


			if(current_obj->getname() == "RobotW"){
				RobotW *robot = dynamic_cast<RobotW *>(current_obj); 
				if(verbose){
					cout<<"Watson meet RobotW"<<endl; 
				}
				if(watson->getHp() > 350){
					bool check = watsonbag->insert(robot->getItem());
					if(check){
						if(verbose){
							cout<<"Watson defeat RobotW and get an item from RobotW"<<endl; 
						}
					}else{
						if(verbose){
							cout<<"Watson defeat RobotW and can't get an item from RobotW because the bag is full"<<endl; 
						}
					}
				}else{
					int current_hp_watson = watson->getHp();
					int hp_watson_after = (int) current_hp_watson*0.95; 
					BaseItem * item = watsonbag->get(ItemType::PASSING_CARD);
					if(item == NULL){
						watson->setHp(hp_watson_after); 
					}else{
						PassingCard *passingcard = dynamic_cast<PassingCard *>(item); 
						if(passingcard->canUse(watson, robot)){
							passingcard->use(watson, robot);
							if(verbose){
								cout<<"Watson using passing card and overcome the challenge"<<endl; 
							}
						}else{
							watson->setHp(hp_watson_after); 
						}
					}
				}
			}
			if(current_obj->getname() == "RobotSW"){
				RobotSW *robot = dynamic_cast<RobotSW *>(current_obj); 
				if(verbose){
					cout<<"Sherlock meet RobotSW"<<endl; 
				}
				if((watson->getExp() > 600)&&(watson->getHp()>165)){
					bool check = sherlockbag->insert(robot->getItem()); 
					if(check){
						if(verbose){
							cout<<"Sherlock defeat RobotS and get an item from RobotSW ."<<endl;
						}
					}else{
						if(verbose){
							cout<<"Sherlock defeat RobotS and can't get an item from RobotSW because the bag is full."<<endl;
						}
					}
				}else{
					int current_hp_watson = watson->getHp();
					int hp_watson_after = (int) current_hp_watson*0.85;

					int current_exp_watson = watson->getExp();
					int exp_watson_after = (int) current_exp_watson*0.85;  
					BaseItem * item = watsonbag->get(ItemType::PASSING_CARD);
					if(item == NULL){
						watson->setHp(hp_watson_after); 
						watson->setExp(exp_watson_after); 
					}else{
						PassingCard *passingcard = dynamic_cast<PassingCard *>(item); 
						if(passingcard->canUse(watson, robot)){
							passingcard->use(watson, robot);
							if(verbose){
								cout<<"Watson using passing card and overcome the challenge"<<endl; 
							}
						}else{
							watson->setHp(hp_watson_after); 
							watson->setExp(exp_watson_after);
						}
					}
				}
			}
			/*
				Watson gặp RobotC tức là đã gặp được vị trí liền kề tên tội phạm. Watson
				chưa thể bắt được tên tội phạm vì bị giữ chân bởi RobotC. Tuy vâỵ, Watson vẫn sẽ
				tiêu diệt được robot và nhận về vật phẩm của robot này nắm giữ.
			*/
			if(current_obj->getname() == "RobotC"){
				RobotC *robot = dynamic_cast<RobotC *>(current_obj); 
				if(verbose){
					cout<<"Watson meet RobotC"<<endl; 
				}
				bool check = watsonbag->insert(robot->getItem()); 
				if(check){
					if(verbose){
						cout<<"Watson defeat RobotC and get an item from RobotC"<<endl; 
					}
				}else{
					if(verbose){
						cout<<"Watson defeat RobotC and can't get an item from RobotC"<<endl;
					}
				}
			}
			/*
				Thời điểm thứ hai chỉ có thể sử dụng MagicBook,
				EnergyDrink hoặc FirstAid
			*/
			Node* currentbag = watsonbag->getBag();
			while(currentbag != NULL){

				if(currentbag->item->getItemType() == ItemType::ENERGY_DRINK){
					BaseItem* useitem = watsonbag->get(ItemType::ENERGY_DRINK);
					useitem->use(watson, NULL); 
					cout<<"Watson use an item"<<endl;
					break; 
				}else if(currentbag->item->getItemType() == ItemType::FIRST_AID){
					BaseItem* useitem = watsonbag->get(ItemType::FIRST_AID);
					useitem->use(watson, NULL); 
					cout<<"Watson use an item"<<endl;
					break; 
				}else if(currentbag->item->getItemType() == ItemType::MAGIC_BOOK){
					BaseItem* useitem = watsonbag->get(ItemType::MAGIC_BOOK);
					useitem->use(watson, NULL); 
					cout<<"Watson use an item"<<endl;
					break; 
				}
				currentbag = currentbag->getNext(); 
			}

		}
	}
	// Dieu chinh lai luong hp va exp cua Sherlock
	if (watson->getHp() < 0)
    {
        watson->setHp(0);
    }
    if (watson->getExp() < 0)
    {
        watson->setExp(0);
    }
    if (watson->getHp() > 500)
    {
        watson->setHp(500);
    }
    if (watson->getExp() > 900)
    {
        watson->setExp(900);
    }
}
void StudyPinkProgram::run(bool verbose){
	/*
		Nếu verbose bằng true thì cần in ra
		thông tin của mỗi MovingObject trong ArrayMovingObject sau khi thực hiện một
		bước di chuyển và các cập nhật sau đó nếu có (ví dụ như Watson gặp một Robot và
		thực hiện thử thách với Robot)
	*/
	for(int step = 0; step < config->num_steps; step++){
		criminal->move();
		if(isStop()){
			printStep(step); 
			printResult(); 
			break; 
		}
		/*
			Sau mỗi 3 bước đi của tên tội phạm một robot sẽ được tạo ra tại vị trí trước đó mà
			tên tội phạm đang đứng. Các loại robot và điều kiện tạo ra tương ứng:
			Bài tập lớn môn Kỹ thuật lập trình - HK 2 năm học 2023 - 2024 Trang 13/25TRƯỜNG ĐẠI HỌC BÁCH KHOA - ĐHQG-HCM
			KHOA KHOA HỌC VÀ KỸ THUẬT MÁY TÍNH
			• Nếu là robot đầu tiên được tạo ra trên bản đồ, đó sẽ là loại robot RobotC. Nếu không,
			ta xét khoảng cách từ Robot đến Sherlock và Watson:
			• Nếu khoảng cách đến Sherlock gần hơn: Tạo ra loại robot RobotS
			• Khoảng cách đến Watson gần hơn: Tạo ra loại robot RobotW
			• Khoảng cách đến Sherlock và Watson là bằng nhau: Tạo ra loại robot RobotSW
		*/
		if(step == 2){
			RobotC* RC = new RobotC(arr_mv_objs->size(), criminal->get_prevpos(), map, criminal);
			bool check = arr_mv_objs->add(RC); 
		}else if((step > 2) && (step%3 == 2)){
			Position pos_robot = criminal->get_prevpos(); 
			Position current_pos_sherlock = sherlock->getCurrentPosition(); 
			Position current_pos_watson = watson->getCurrentPosition(); 
			int sherlock_distance = abs(pos_robot.getRow() - current_pos_sherlock.getRow()) + abs(pos_robot.getCol() - current_pos_sherlock.getCol()); 
			int watson_distance = abs(pos_robot.getRow() - current_pos_watson.getRow()) + abs(pos_robot.getCol() - current_pos_watson.getCol()); 
			int index = arr_mv_objs->size(); // index cua robot moi  
			if(sherlock_distance < watson_distance){
				RobotS* RS = new RobotS(index, pos_robot, map, criminal, sherlock);
				bool check_success = arr_mv_objs->add(RS); 
			}else if(sherlock_distance > watson_distance){
				RobotW* RW = new RobotW(index, pos_robot, map, criminal, watson); 
				bool check_success = arr_mv_objs->add(RW); 
			}else if(sherlock_distance == watson_distance){
				RobotSW* RSW = new RobotSW(index, pos_robot, map, criminal, sherlock, watson); 
				bool check_success = arr_mv_objs->add(RSW); 
			}
		}
		sherlock->move();
		sherlockcheckmeet(verbose, 2, arr_mv_objs->size()); 
		if(isStop()){
			printStep(step);
			printResult(); 
			break;  
		}
		watson->move(); 
		watsoncheckmeet(verbose, 3, arr_mv_objs->size()); 
		if(isStop()){
			printStep(step); 
			printResult();
			break; 
		}
		for(int i = 3; i < arr_mv_objs->size(); i++){
			arr_mv_objs->get(i)->move(); 
			sherlockcheckmeet(verbose, i, i + 1);
			watsoncheckmeet(verbose, i, i + 1);
			if(isStop()){
				printStep(step); 
				printResult(); 
				break; 
			}  
		}
		if (verbose)
        {
            cout << arr_mv_objs->str() << endl;
            cout << "Sherlock HP:" << sherlock->getHp() << endl;
            cout << "Sherlock Exp:" << sherlock->getExp() << endl;
            cout << sherlockbag->str() << endl;
            cout << "Watson HP:" << watson->getHp() << endl;
            cout << "Watson Exp:" << watson->getExp() << endl;
            cout << watsonbag->str() << endl;
            printStep(step);
        }
	}
	printResult();
}
/*
	Nếu là robot đầu tiên được tạo ra trên bản đồ, đó sẽ là loại robot RobotC. Nếu không,
	ta xét khoảng cách từ Robot đến Sherlock và Watson:
	• Nếu khoảng cách đến Sherlock gần hơn: Tạo ra loại robot RobotS
	• Khoảng cách đến Watson gần hơn: Tạo ra loại robot RobotW
	• Khoảng cách đến Sherlock và Watson là bằng nhau: Tạo ra loại robot RobotSW
*/
StudyPinkProgram::~StudyPinkProgram(){
    delete config;
    delete sherlock;
    delete watson;
    delete criminal;
    delete map;
    delete arr_mv_objs;
    delete sherlockbag;
    delete watsonbag;
    delete robot;
}