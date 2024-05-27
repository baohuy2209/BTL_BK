#include <iostream>
#include <iomanip>
#include <cmath>
#include <cstring>
#include <climits>
#include <cassert>
#include <string>
#include <fstream>
#include <sstream>

using namespace std;

class MovingObject;
class Position;
class Configuration;
class Map;

class Criminal;
class RobotS;
class RobotW;
class RobotSW;
class RobotC;

class ArrayMovingObject;
class StudyPinkProgram;

class BaseItem;
class BaseBag;
class SherlockBag;
class WatsonBag;

class TestStudyInPink;

enum ItemType
{
    MAGIC_BOOK,
    ENERGY_DRINK,
    FIRST_AID,
    EXCEMPTION_CARD,
    PASSING_CARD
};
enum ElementType
{
    PATH,
    WALL,
    FAKE_WALL
};
enum RobotType
{
    C,
    S,
    W,
    SW
};

class MapElement{
	friend class TestStudyInPink; 
	protected: 
		ElementType type; // Kieu thanh thanh ban do 
	public: 
		// constructor
		MapElement(ElementType in_type){
			this->type = in_type; 
		}
		// destructor
		virtual ~MapElement(){}
		// phuong thuc lay gia tri cua type 
		virtual ElementType getType() const {return type;}
};
class Path: public MapElement{
	friend class TestStudyInPink; 
	public: 
		// contructor 
		Path(): MapElement(PATH){} 
};

class Wall: public MapElement{
	friend class TestStudyInPink; 
	public: 
		// constructor 
		Wall(): MapElement(WALL){}
};
/*
	biểu diễn một bức tường giả, tên tội phạm vì là người tạo ra mê cung nên
	nhận biết được tường giả, còn Sherlock bằng khả năng quan sát của mình thì có thể phát
	hiện được tường giả này. Đối với Watson, FakeWall sẽ bị phát hiện (và di chuyển qua
	được) nếu Watson có EXP lớn hơn EXP yêu cầu của FakeWall.
*/
class FakeWall: public MapElement{
	friend class TestStudyInPink; 
	private: 
		int req_exp; // EXP toi thieu ma Waison can co de phat hien ra buc tuong
	public: 
		// constructor 
		FakeWall(int in_reg_exp): MapElement(FAKE_WALL){
			req_exp = in_reg_exp; 
		}
		int getReqExp() const {
			return this->req_exp; 
			/*
				(r * 257 + c * 139 + 89)%900 + 1
				r va c lan luot la vi tri theo hang va cot cua FakeWall
			*/
		}
};
class Position
{
    friend class TestStudyInPink;

	private:
		int r, c;

	public:
		Position(int r = 0, int c = 0){
			this->r = r; 
			this->c = c; 
		}
		Position(const string& str_pos){
			sscanf(str_pos.c_str(), "(%d,%d)", &this->r, &this->c);
		}
		int getRow() const;
		int getCol() const;
		void setRow(int r);
		void setCol(int c);
		string str() const;
		bool isEqual(int in_r, int in_c) const;
		bool isEqual(Position pos) const; 
		static const Position npos;
};
class MovingObject{
    friend class TestStudyInPink;
	protected: 
		int index; // vi tri cua doi tuong di chuyen trong mang cac doi tuong di chuyen, mang nay se mo to sau 
		Position pos; // vi tri hien tai cua doi tuong 
		Map *map; // ban do cho doi tuong nay di chuyen trong do 
		string name; // doi tuong di chuyen
	public: 
		MovingObject(int index, const Position pos, Map * map, const string &name=""){
			this->index = index; 
			this->pos = pos; 
			this->map = map; 
			this->name = name; 
		}
		virtual ~MovingObject(){}
		/*
			Phuong thuc ao thuan to (pure virtual method) getNextPosition tra ve Position tiep theo ma 
			doi tuong nay di chuyen den. Mat khac, trong truong hop khong co Position nao de doi tuong di chuyen den, ta dinh nghia mot gia tri de tra ve cho phuong thuc nay va luu trong bien npos cua class Position. 
			Khi khong co Position de di chuyen den thi phuong thuc tra ve npos. 
		*/
		virtual Position getNextPosition() = 0; 
		Position getCurrentPosition() const;
		string getName(){
			return this->name; 
		}
		// Method move thuc hien 1 buoc di chuyen cua doi tuong 
		virtual void move() = 0; 
		// Pure virutal method str tra ve chuoi bieu dien thong tin cua doi tuong 
		virtual string str() const = 0; 
};
class Map
{
    friend class TestStudyInPink;

private:
    int num_rows, num_cols;
    MapElement ***map;

public:
    Map(int num_rows, int num_cols, int num_walls, Position *array_walls, int num_fake_walls, Position *array_fake_walls);
    ~Map();
    bool isValid(const Position &pos, MovingObject *mv_obj) const;
};

class Character: public MovingObject{
	friend class TestStudyInPink; 
	public: 
		int hp; 
		int exp; 
		Character(int index, const Position pos, Map * map, const string & name , int init_hp, int init_exp): MovingObject(index, pos, map, name){
			this->hp = init_hp; 
			this->exp = init_exp; 
		}
		~Character(){} 
		virtual Position getNextPosition() override = 0;
		Position getCurrentPosition() const{
			return this->pos;
		}
		void setCurrentPosition(Position pos){
			this->pos = pos; 
		}
		virtual void move() override = 0; 
		virtual string str() const override = 0; 
		int getHp(){return this->hp;}
		int getExp(){return this->exp;}
		void setExp(int exp){this->exp = exp;}
		void setHp(int hp){this->hp = hp;}
}; 
class Sherlock : public Character
{
    friend class TestStudyInPink;

    private:
        // TODO
        string moving_rule;
		int global_count; 
    public:
		/*
			- moving rule: mô tả cách thức mà Sherlock di chuyển. 
				Đây là một chuỗi mà các ký tự chỉ có thể là một trong 4 giá trị: 
				 + 'L' (Left - đi sang trái) 
				 + 'R' (Right - đi sang phải)
				 + 'U' (Up - đi lên)
				 + 'D' (Down - đi xuống dưới)
				Ví dụ về moving_rule là "LU". 
			- init_hp: HP ban đầu của SherLock. HP nằm trong khoảng [0,500]. Nếu HP vượt quá 500 thì được cài đặt về 500, 
				nếu HP bằng 0 thì coi như SherLock đã hết thể lực và không thể di chuyến tiếp trong mê cung. 
				Nếu HP của cả SherLock và Watson đều bằng 0 thì SherLock và Watson bị thua trong cuộc truy đuổi với tên tội phạm. 
			- init_exp: EXP ban đầu của SherLock. EXP nằm trong khoảng [0, 900]. Nếu HP vượt quá 900 thì cài đặt về 900, 
				nếu EXP bằng 0 thì SherLock cũng sẽ không di chuyển tiếp trong mê cung. 
			- Tham số name của Constructor MovingObject được truyền vào giá trị "Sherlock". 
			- SherLock có thêm các thuộc tính hp và exp; 
  		*/
        Sherlock(int index, const string & moving_rule, const Position &init_pos, Map*map, int init_hp, int init_exp) : Character(index, init_pos, map, "Sherlock", init_hp, init_exp){
			this->moving_rule = moving_rule; 
			// khoi tao hp 
			if(init_hp > 500){
				this->hp = 500; 
			}else if(init_hp < 0){
				this->hp = 0; 
			}
			// khoi tao exp 
			if(init_exp > 900){
				this->exp = 900; 
			}else if(init_exp < 0){
				this->exp = 0; 
			}else{
				this->exp = init_exp; 	
			}
			global_count = 0; 
		}
        virtual Position getNextPosition() override;
        virtual void move() override;
        virtual string str() const override;
    // ...
};

class Watson : public Character
{
    friend class TestStudyInPink;

    private:
        // TODO
        string moving_rule;
		int global_count; 
    public:
        Watson(int index, const string & moving_rule, const Position &init_pos, Map*map, int init_hp, int init_exp) : Character(index, init_pos, map, "Watson", init_hp, init_exp){
			this->moving_rule = moving_rule; 
			// khoi tao hp 
			if(init_hp > 500){
				this->hp = 500; 
			}else if(init_hp < 0){
				this->hp = 0; 
			}else{
				this->hp = init_hp;	
			} 
			// khoi tao exp 
			if(init_exp > 900){
				this->exp = 900; 
			}else if(init_exp < 0){
				this->exp = 0; 
			}else{
				this->exp = init_exp; 	
			}
			global_count = 0; 
		}
        virtual Position getNextPosition() override;
        virtual void move() override;
        virtual string str() const override;
        int getExp() const { return this->exp; }
};

class Criminal : public Character
{
    friend class TestStudyInPink;

    private:
        // TODO
        Sherlock *sherlock;
        Watson *watson;
        Position previous_pos;

    public:
        Criminal(int index, const Position & init_pos, Map * map, Sherlock * sherlock, Watson * watson) : Character(index, init_pos, map, "Criminal", 0, 0){
			this->sherlock = sherlock; 
			this->watson = watson; 
		}
		~Criminal(){
			delete sherlock; 
			delete watson;
		}
        Position getPrevious_pos(); 
        int getDistanceObj(MovingObject * obj1, Position pos);
        virtual Position getNextPosition() override;
        virtual void move() override;
        virtual string str() const override;
    // ...
};

class ArrayMovingObject
{
    friend class TestStudyInPink;

    private:
        // TODO
        int count;
        int capacity;
        MovingObject **arr_mv_objs;

    public:
        ArrayMovingObject(int capacity){
			this->capacity = capacity; 
			this->count = 0; 
			this->arr_mv_objs = new MovingObject*[capacity]; 
		}
		~ArrayMovingObject(){
			delete[] arr_mv_objs; 
		}
        bool isFull() const;
        bool add(MovingObject* mv_obj);
        MovingObject* get(int index) const;
        int size() const; // tra ve kich thuoc hien tai 
        string str() const;
};

class Configuration
{
    friend class TestStudyInPink;
    friend class StudyPinkProgram;

private:
    // TODO
    int map_num_rows;
    int map_num_cols;
    int max_num_moving_objects;
    int num_walls;
    Position *arr_walls;
    int num_fake_walls;
    Position *arr_fake_walls;
    string sherlock_moving_rule;
    Position sherlock_init_pos;
    int sherlock_init_hp;
    int sherlock_init_exp;
    string watson_moving_rule;
    Position watson_init_pos;
    int watson_init_hp;
    int watson_init_exp;
    Position criminal_init_pos;
    int num_steps;

public:
    Configuration(const string &filepath);
    ~Configuration();
    string str() const;
};
// Robot, BaseItem, BaseBag,...
class Robot : public MovingObject
{
    friend class TestStudyInPink;

    protected:
		RobotType robot_type; 
        BaseItem *item;
    public:
        Robot(int index, const Position init_pos, Map * map, const string &name, RobotType robot_type); 
        virtual Position getNextPosition() override = 0;
        Position getCurrentPosition() const{
			return this->pos; 
		};
        virtual void move() override = 0;
		BaseItem * getItem(){
			return this->item; 
		}
        virtual string str() const override = 0;
		string getName() const;
};
class RobotC : public Robot
{
    friend class TestStudyInPink;
	private: 
		Criminal * criminal; 
    public:
        RobotC(int index, const Position init_pos, Map* map, Criminal* criminal): Robot(index, init_pos, map,"RobotC", RobotType::C){
			this->criminal = criminal; 
		} 
        virtual Position getNextPosition() override;
        virtual void move() override;
        int getDistance(MovingObject * moving_object) const;
        RobotType getType() const{
			return this->robot_type; 
		}
        virtual string str() const override;
};

class RobotS : public Robot
{
    friend class TestStudyInPink;

    private:
        Sherlock *sherlock;
		Criminal *criminal;
    public:
        RobotS(int index, const Position init_pos, Map *map, Criminal* criminal, Sherlock* sherlock): Robot(index, init_pos, map, "RobotS", RobotType::S){
			this->sherlock = sherlock; 
			this->criminal = criminal; 
        } 
        virtual Position getNextPosition() override;
        virtual void move() override;
        int getDistance() const;
        virtual string str() const override;
        RobotType getType() const{
			return this->robot_type; 
		}
};

class RobotW : public Robot
{
    friend class TestStudyInPink;

    private:
        Watson *watson;
		Criminal *criminal; 
    public:
        RobotW(int index, const Position init_pos, Map * map, Criminal* criminal, Watson* watson): Robot(index, init_pos, map,"RobotW", RobotType::W){
			this->watson = watson; 
			this->criminal = criminal; 
        }
        virtual Position getNextPosition() override;
        virtual void move() override;
        int getDistance() const ;
        virtual string str() const override;
        RobotType getType() const{
			return this->robot_type; 
		}
};

class RobotSW : public Robot
{
    friend class TestStudyInPink;

	private:
		Sherlock *sherlock;
		Watson *watson;
		Criminal *criminal; 
	public:
		RobotSW(int index, const Position init_pos, Map * map, Criminal* criminal, Sherlock* sherlock, Watson* watson): Robot(index, init_pos, map,"RobotSW", RobotType::SW){
			this->sherlock = sherlock; 
			this->watson = watson; 
			this->criminal = criminal; 
        }
		virtual Position getNextPosition() override;
		virtual void move() override;
		int getDistance() const ;
		virtual string str() const override;
		RobotType getType() const{
			return this->robot_type; 
		}
};
class BaseItem
{
    friend class TestStudyInPink;

    protected:
        ItemType item_type;
    public:
        BaseItem(ItemType type){
			this->item_type = type; 
		}
		virtual ~BaseItem(); 
        ItemType getItemType(){
			return this->item_type; 
		}; 
        void setType(ItemType type){
			this->item_type = type; 
		}
        virtual bool canUse(Character *obj, Robot *robot) = 0;
        virtual void use(Character *obj, Robot *robot) = 0;
};
class MagicBook : public BaseItem
{
    friend class TestStudyInPink;

	public:
		MagicBook() : BaseItem(ItemType::MAGIC_BOOK){};
		bool canUse(Character *obj, Robot *robot) override;
		void use(Character *obj, Robot *robot) override;
};
class EnergyDrink : public BaseItem
{
    friend class TestStudyInPink;

	public:
		EnergyDrink() : BaseItem(ItemType::ENERGY_DRINK){};
		bool canUse(Character *obj, Robot *robot) override;
		void use(Character *obj, Robot *robot) override;
};
class FirstAid : public BaseItem
{
    friend class TestStudyInPink;

	public:
		FirstAid() : BaseItem(ItemType::FIRST_AID){};
		bool canUse(Character *obj, Robot *robot) override;
		void use(Character *obj, Robot *robot) override;
};
class ExcemptionCard : public BaseItem
{
    friend class TestStudyInPink;

	public:
		ExcemptionCard() : BaseItem(ItemType::EXCEMPTION_CARD){};
		bool canUse(Character *obj, Robot *robot) override;
		void use(Character *obj, Robot *robot) override;
};
class PassingCard : public BaseItem
{
    friend class TestStudyInPink;
	private: 
    	string challenge; 
	public:
		PassingCard(string challenge): BaseItem(ItemType::PASSING_CARD){
    		this->challenge = challenge; 
		}
		virtual bool canUse(Character *obj, Robot *robot) override;
		virtual void use(Character *obj, Robot *robot) override;
		string getChallenge(){
			return this->challenge; 
		}
};

class Node{
    public:
    	BaseItem* item; 
        Node* next; 
        Node(BaseItem* item){
            this->item = item; 
            this->next = NULL;
        }
        BaseItem* getItem(){return this->item;}
        void setItem(BaseItem* item) {this->item = item;}
        
        Node* getNext() {return this->next;}
		void setNext(Node* next){this->next = next;}
};
class BaseBag
{
    friend class TestStudyInPink;

	protected:
		Character *obj;

	public:
		BaseBag(Character* obj){
    		this->obj = obj; 
		}
		virtual ~BaseBag(); 
		virtual bool insert(BaseItem *item) = 0;
		virtual BaseItem *get() = 0;
		virtual BaseItem *get(ItemType itemType) = 0;
		virtual string str() const = 0;
};
class SherlockBag : public BaseBag
{
    friend class TestStudyInPink;
	private: 
        int capacity; 
        Node* bag; 
	public:
		int countItem() const;
		SherlockBag(Sherlock* sherlock): BaseBag(sherlock){
            this->capacity = 13; 
            this->bag = NULL;
        }
		bool insert(BaseItem * item) ;
		Node* getBag(){
			return this->bag; 
		}
		BaseItem* get();
		BaseItem* get(ItemType itemType);
		string convert_int_to_string (int a) const{
			char mychar[1000];
			sprintf(mychar, "%d", a); 
			string str(mychar); 
			return str; 
		}
		string str() const override;
};
class WatsonBag : public BaseBag
{
    friend class TestStudyInPink;
	private: 
        int capacity; 
        Node* bag;
	public:
		int countItem() const;
		WatsonBag(Watson* watson): BaseBag(watson){
            this->capacity = 15; 
            this->bag = NULL; 
        }
		bool insert(BaseItem * item) override;
		Node* getBag(){
			return this->bag; 
		}
		BaseItem* get() override;
		BaseItem* get(ItemType itemType) override;
		string convert_int_to_string (int a) const{
			char mychar[1000];
			sprintf(mychar, "%d", a); 
			string str(mychar); 
			return str; 
		}
		string str() const override;
};
class StudyPinkProgram
{
    friend class TestStudyInPink;

	private:
		// Sample attributes
		Configuration *config;
		Sherlock *sherlock;
		Watson *watson;
		Criminal *criminal;
		Map *map;
		ArrayMovingObject *arr_mv_objs;
		SherlockBag *sherlockbag;
		WatsonBag *watsonbag;
		Robot *robot;

	public:
		StudyPinkProgram(const string &config_file_path);
		bool isStop() const;
		void printResult() const {
			if (sherlock->getCurrentPosition().isEqual(criminal->getCurrentPosition())) {
				cout << "Sherlock caught the criminal" << endl;
			}
			else if (watson->getCurrentPosition().isEqual(criminal->getCurrentPosition())) {
				cout << "Watson caught the criminal" << endl;
			}
			else {
				cout << "The criminal escaped" << endl;
			}
		}
		void printStep(int si) const {
			cout << "Step: " << setw(4) << setfill('0') << si
				<< "--"
				<< sherlock->str() << "--|--" << watson->str() << "--|--" << criminal->str() << endl;
		}
		void sherlockcheckmeet(bool verbose, int current_index, int goal_index);
		void watsoncheckmeet(bool verbose, int current_index, int goal_index);
		void run(bool verbose);
		~StudyPinkProgram();
};
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
        if (mv_obj->getName() == "Watson")
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

const Position Position::npos(-1,-1); 
string Position::str() const{
	const string result = "("+to_string(r)+", "+to_string(c)+")"; 
	return result; 
}		
int Position::getRow() const {return this->r;};
int Position::getCol() const {return this->c;};
void Position::setRow(int r) {this->r = r;}
void Position::setCol(int c) {this->c = c;}
bool Position::isEqual(int in_r, int in_c) const{
	// tao bien flag co gia tri mac dinh false, tra ve bien do neu gia tri truyen vao khong trung voi vi tri cua doi tuong nay 
	if((this->c == in_c) && (this->r == in_r)){
		return true; 
	} 
	return false; 
}
bool Position::isEqual(Position pos) const{
	int r_pos = pos.getRow(); 
	int c_pos = pos.getCol(); 
	if((this->r == r_pos) && (this->c == c_pos)){
		return true; 
	}
	return false; 
}
Position MovingObject::getCurrentPosition() const{
	return this->pos; 
}; 
/*
Phương thức getNextPosition (public) trả về vị trí di chuyển tiếp theo của Sherlock.
Sherlock di chuyển dựa theo moving_rule. Mỗi lần gọi phương thức, một ký tự tiếp
theo được sử dụng để làm hướng di chuyển. Lần đầu tiên gọi phương thức thì ký tự đầu
tiên sẽ được sử dụng. Khi ký tự cuối cùng được sử dụng thì sẽ quay lại bắt đầu quá trình
này từ ký tự đầu tiên. Ví dụ với moving_rule = "LR" thì thứ tự các ký tự được sử
dụng là: ’L’, ’R’, ’L’, ’R’, ’L’, ’R’,... Nếu Position được trả ra không phải là một vị trí
hợp lệ cho đối tượng này di chuyển thì trả về npos thuộc class Position.
*/


Position Sherlock::getNextPosition(){
	string temp = this->moving_rule;
	if (moving_rule.empty() || global_count < 0 || global_count >= moving_rule.size())
    {
        return Position::npos;
    }
	// lay lenh di chuyen 
	char command = temp[global_count]; 
	Position current_pos = this->pos;  
	int new_r = current_pos.getRow(); 
	int new_c = current_pos.getCol(); 
	switch(command){
		case 'L':
			new_c = new_c - 1; 
			break; 
		case 'R': 
			new_c = new_c + 1; 
			break; 
		case 'U':
			new_r = new_r-1; 
			break; 
		case 'D':
			new_r = new_r+1; 
			break;
	} 
	Position result(new_r, new_c); 
	if(this->map->isValid(result, this)){
		return result;
	}else{
		return Position::npos;
	}
}
/*
	Phương thức move (public) thực hiện một bước di chuyển của Sherlock. Trong move có
	lời gọi đến getNextPosition, nếu nhận được giá trị trả về khác npos là bước đi hợp lệ
	thì Sherlock sẽ di chuyển đến đó. Nếu không phải bước đi hợp lệ thì Sherlock sẽ đứng im
*/
void Sherlock::move()
{
    Position nepos = getNextPosition();
    global_count++;
    if (!nepos.isEqual(-1, -1) && hp != 0)
    {
        this->pos = nepos;
    }
}
// kiem tra vi tri co lech ra khoi ban do hay khong
string Sherlock::str() const{
	return "Sherlock[index="+to_string(index)+";pos="+pos.str()+";moving_rule="+moving_rule+"]";
}

Position Watson::getNextPosition(){
	string temp = this->moving_rule;
	if(temp[global_count] == '\0'){
		global_count = 0; 
	} 
	// lay lenh di chuyen 
	char command = temp[global_count]; 
	Position current_pos = this->pos;  
	int new_r = current_pos.getRow(); 
	int new_c = current_pos.getCol(); 
	switch(command){
		case 'L':
			new_c = new_c - 1; 
			break; 
		case 'R': 
			new_c = new_c + 1; 
			break; 
		case 'U':
			new_r = new_r-1; 
			break; 
		case 'D':
			new_r = new_r+1; 
			break;
	} 
	Position result(new_r, new_c); 
	if(this->map->isValid(result, this)){
		return result;
	}else{
		return Position::npos;
	}
}
void Watson::move(){
	Position result = getNextPosition(); 
	global_count++; 
	if(!result.isEqual(Position::npos.getRow(), Position::npos.getCol())){
		this->pos = result; 
	}
}
/*
	Phương thức str trả về chuỗi có định dạng như sau:
Watson[index=<index>;pos=<pos>;moving_rule=<moving_rule>]
Các thành phần có ý nghĩa tương tự như trong class Sherlock.
*/
string Watson::str() const{
	return "Watson[index="+to_string(index)+";pos="+pos.str()+";moving_rule="+moving_rule+"]";  
}


Position Criminal::getPrevious_pos(){
	return this->previous_pos; 
}
int Criminal::getDistanceObj(MovingObject * obj1, Position pos){
	Position first_pos = obj1->getCurrentPosition(); 
	// lay toa do 
	int x1 = first_pos.getRow();
	int y1 = first_pos.getCol(); 
	
	int x2 = pos.getRow(); 
	int y2 = pos.getCol(); 
	
	int result = abs(x1-x2) + abs(y1-y2); 
	return result; 
}
enum Direction{
	UP, LEFT, DOWN, RIGHT
};
Position Criminal::getNextPosition(){
	Position currentpos = this->pos; 
	// Cac vi tri toi pham chuan bi di chuyen den
	Position * move_pos = new Position[4]; 
	move_pos[RIGHT] = Position(currentpos.getRow(), currentpos.getCol()+1);
	move_pos[LEFT] = Position(currentpos.getRow(), currentpos.getCol()-1); 
	move_pos[UP] = Position(currentpos.getRow()-1, currentpos.getCol()); 
	move_pos[DOWN] = Position(currentpos.getRow()+1, currentpos.getCol()); 
	
	
	// lay gia tri khoang cach 
	int * distance = new int[4]; 
	for(int i = 0; i<4; i++){
		if(map->isValid(move_pos[i], this)){
			distance[i] = getDistanceObj(this->sherlock, move_pos[i]) + getDistanceObj(this->watson, move_pos[i]);
		}else{
			distance[i] = 0; 
		}
	}
	// tim khoang cach lon nhat 
	int max_distance = distance[0]; 
	int temp = 0; 
	for(int i = 0; i<4; i++){
		if(max_distance < distance[i]){
			max_distance = distance[i]; 
			temp = i; 
		}
	}
	if(max_distance == 0){
		return Position::npos; 
	}
	Position result = move_pos[temp]; 
	delete []distance; 
	delete []move_pos;
	if(this->map->isValid(result, this)){
		return result; 
	}else{
		return Position::npos; 	
	}
}
void Criminal::move(){
	Position result = getNextPosition(); 
	if(!result.isEqual(Position::npos.getRow(), Position::npos.getCol())){
		this->previous_pos = this->pos; 
		this->pos = result; 
	}
}

/*
	Phuong th?c str tr? v? chu?i c� d?nh d?ng nhu sau:
	Criminal[index=<index>;pos=<pos>]
*/
string Criminal::str() const{
	return "Criminal[index="+to_string(index)+";pos="+pos.str()+"]"; 
}

bool ArrayMovingObject::isFull() const{
	if(this->count != this->capacity){
		return false; 
	}
	return true; 
}
bool ArrayMovingObject::add(MovingObject* mv_obj){
	if(!isFull()){
		int idx = this->count;
		arr_mv_objs[idx] = mv_obj; 
		this->count += 1; 
		return true; 
	}else{
		return false; 
	}
}
MovingObject* ArrayMovingObject::get(int index) const{
	return this->arr_mv_objs[index]; 
}
int ArrayMovingObject::size() const{
	return count; 
}
string ArrayMovingObject::str() const{
	
	string result="ArrayMovingObject[count="+to_string(count)+";capacity="+to_string(capacity); 
	for(int i = 0; i < this->count; i++){
		string temp = ";"+arr_mv_objs[i]->str();
		result = result + temp; 
	}
	result += "]";
	
	return result; 
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
	Position new_pos = this->criminal->getPrevious_pos(); 
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
	if(obj->exp <= 350){
		flag = true; 
		return flag; 
	}
	return flag; 
}
void MagicBook::use( Character * obj , Robot * robot ){
	int current_exp = obj->exp; 
	obj->exp = current_exp + int(current_exp*0.25); 
}
bool FirstAid::canUse ( Character * obj , Robot * robot ){
	if((obj->hp <= 100) || (obj->exp <= 350)) {
		return true; 
	}
	return false; 
}
void FirstAid::use( Character * obj , Robot * robot ){
	int current_hp = obj->hp; 
	obj->hp = current_hp + int(current_hp*0.5);
}
bool ExcemptionCard::canUse ( Character * obj , Robot * robot ){
	if((obj->getName() == "Sherlock") && (obj->getHp()%2 == 1)){
		return true; 
	}
	return false; 
}
void ExcemptionCard::use( Character * obj , Robot * robot ){

}
bool EnergyDrink::canUse ( Character * obj , Robot * robot ){
	if(obj->hp <= 100){
		return true; 
	}
	return false; 
}
void EnergyDrink::use( Character * obj , Robot * robot ){
	int current_hp = obj->hp; 
	obj->hp = current_hp + int(current_hp*0.2); 
}
bool PassingCard::canUse ( Character * obj , Robot * robot ){
	if(obj->getName() == "Watson" && (obj->hp%2 == 0)){
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
		obj->exp = obj->exp - 50; 
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


			if(current_obj->getName() == "RobotS"){
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


			if(current_obj->getName() == "RobotW"){
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
			if(current_obj->getName() == "RobotSW"){
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
			if(current_obj->getName() == "RobotC"){
				RobotC *robot = dynamic_cast<RobotC *>(current_obj); 
				if(verbose){
					cout<<"Sherlock meet RobotC"<<endl; 
				}
				if(sherlock->getExp() > 500){
					Position current_pos_criminal = criminal->getCurrentPosition();
					sherlock->setCurrentPosition(current_pos_criminal);
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

			if(current_obj->getName() == "RobotS"){ 
				if(verbose){
					cout<<"Watson meet RobotS"<<endl; 
				}
			}


			if(current_obj->getName() == "RobotW"){
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
			if(current_obj->getName() == "RobotSW"){
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
			if(current_obj->getName() == "RobotC"){
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
			RobotC* RC = new RobotC(arr_mv_objs->size(), criminal->getPrevious_pos(), map, criminal);
			bool check = arr_mv_objs->add(RC); 
		}else if((step > 2) && (step%3 == 2)){
			Position pos_robot = criminal->getPrevious_pos(); 
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

void tc_1001() {
    cout << "----- Testcase 01 -----" << endl;
    Path path = Path();
    cout << path.getType();
}
void tc_1004() {
    cout << "----- Testcase 04 -----" << endl;
    Wall wall = Wall();
    cout << wall.getType();
}
void tc_1009() {
    cout << "----- Testcase 09 -----" << endl;
    FakeWall *fakewall = new FakeWall(100);
    cout << fakewall->getType();
    delete fakewall;
}
void tc_1014() {
    cout << "----- Testcase 14 -----" << endl;
    int num_walls = 2;
    Position arr_walls[] = {Position(2, 3), Position(3, 0)};
    int num_fake_walls = 2;
    Position arr_fake_walls[] = {Position(2, 0), Position(2, 4)};
    Map * map = new Map(5, 5, num_walls, arr_walls, num_fake_walls, arr_fake_walls);
}
void tc_1015() {
    cout << "----- Testcase 15 -----" << endl;
    int num_walls = 3;
    Position arr_walls[] = {Position(2, 4), Position(2, 3), Position(3, 1)};
    int num_fake_walls = 1;
    Position arr_fake_walls[] = {Position(1, 1)};
    Map * map = new Map(7, 7, num_walls, arr_walls, num_fake_walls, arr_fake_walls);
}
void tc_1016() {
    cout << "----- Testcase 16 -----" << endl;
    int num_walls = 4;
    Position arr_walls[] = {Position(1, 2), Position(2, 3), Position(3, 4), Position(4, 3)};
    for(int i = 0; i < num_walls; i++) {
        cout<<arr_walls[i].str() << endl;
    }
    int num_fake_walls = 1;
    Position arr_fake_walls[] = {Position(3, 3)};
    for(int i = 0; i < num_fake_walls; i++) {
        cout<<arr_fake_walls[i].str() <<endl;
    }
    Map * map = new Map(7, 5, num_walls, arr_walls, num_fake_walls, arr_fake_walls);
}
void tc_1017() {
    cout << "----- Testcase 17 -----" << endl;
    int num_walls = 1;
    Position arr_walls[] = {Position(4, 0)};
    int num_fake_walls = 1;
    Position arr_fake_walls[] = {Position(0, 0)};
    Map * map = new Map(6, 2, num_walls, arr_walls, num_fake_walls, arr_fake_walls);
}
void tc_1057() {
    cout << "----- Testcase 57 -----" << endl;
    Position pos = Position("(0, 0)");
    cout << pos.str();
}
void tc_1062() {
    cout << "----- Testcase 62 -----" << endl;
    Position pos = Position("(3, 5)");
    cout << pos.getRow();
}
void tc_1063() {
    cout << "----- Testcase 63 -----" << endl;
    Position pos = Position(2, 4);
    cout << pos.getRow();
}
void tc_1066() {
    cout << "----- Testcase 66 -----" << endl;
    Position pos = Position("(3, 5)");
    pos.setRow(5);
    cout << pos.str();
}
void tc_1067() {
    cout << "----- Testcase 67 -----" << endl;
    Position pos = Position(2, 4);
    pos.setRow(3);
    cout << pos.str();
}
void tc_1070() {
    cout << "----- Testcase 70 -----" << endl;
    Position pos = Position(2, 4);
    cout << pos.isEqual(2, 4);
}
void tc_1094() {
    cout << "----- Testcase 94 -----" << endl;
    int num_walls = 4;
    Position arr_walls[] = {Position(1, 1), Position(1, 3), Position(3, 1), Position(3, 3)};
    int num_fake_walls = 1;
    Position arr_fake_walls[] = {Position(2, 2)};

    Map * map = new Map(5, 5, num_walls, arr_walls, num_fake_walls, arr_fake_walls);

    Sherlock * sherlock = new Sherlock(1, "URL", Position(1, 2), map, 100, 250);
    cout << sherlock->getNextPosition().str() << endl;

    delete map;
    delete sherlock;
}
void tc_1099() {
    cout << "----- Testcase 99 -----" << endl;
    int num_walls = 4;
    Position arr_walls[] = {Position(1, 1), Position(1, 3), Position(3, 1), Position(3, 3)};
    int num_fake_walls = 1;
    Position arr_fake_walls[] = {Position(2, 2)};

    Map * map = new Map(5, 5, num_walls, arr_walls, num_fake_walls, arr_fake_walls);

    Sherlock * sherlock = new Sherlock(1, "URL", Position(1, 2), map, 100, 250);
    sherlock->move();
    cout << sherlock->str();

    delete map;
    delete sherlock;
}
void tc_1109() {
    cout << "----- Testcase 109 -----" << endl;
    int num_walls = 4;
    Position arr_walls[] = {Position(1, 1), Position(1, 3), Position(3, 1), Position(3, 3)};
    int num_fake_walls = 1;
    Position arr_fake_walls[] = {Position(2, 2)};

    Map * map = new Map(5, 5, num_walls, arr_walls, num_fake_walls, arr_fake_walls);

    Sherlock * sherlock = new Sherlock(1, "URL", Position(1, 2), map, 100, 250);
    cout << sherlock->str() << endl;

    delete map;
    delete sherlock;
}
void tc_1114() {
    cout << "----- Testcase 114 -----" << endl;
    int num_walls = 8;
    Position arr_walls[] = {Position(1, 1), Position(2, 1), Position(3, 1), Position(4, 1), Position(1, 3), Position(2, 3), Position(3, 3), Position(4, 3)};
    int num_fake_walls = 2;
    Position arr_fake_walls[] = {Position(1, 2), Position(4, 2)};

    Map * map = new Map(5, 5, num_walls, arr_walls, num_fake_walls, arr_fake_walls);

    Watson *watson = new Watson(1, "LUR", Position(0, 1), map, 100, 250);
    cout << watson->getNextPosition().str() << endl;

    delete map;
    delete watson;
}
void tc_1120() {
    cout << "----- Testcase 120 -----" << endl;
    int num_walls = 8;
    Position arr_walls[] = {Position(1, 1), Position(2, 1), Position(3, 1), Position(4, 1), Position(1, 3), Position(2, 3), Position(3, 3), Position(4, 3)};
    int num_fake_walls = 2;
    Position arr_fake_walls[] = {Position(1, 2), Position(4, 2)};

    Map * map = new Map(5, 5, num_walls, arr_walls, num_fake_walls, arr_fake_walls);

    Watson *watson = new Watson(1, "LUR", Position(0, 1), map, 100, 250);
    watson->move();
    cout << watson->str();

    delete map;
    delete watson;
}
void tc_1132() {
    cout << "----- Testcase 132 -----" << endl;
    int num_walls = 8;
    Position arr_walls[] = {Position(1, 1), Position(2, 1), Position(3, 1), Position(4, 1), Position(1, 3), Position(2, 3), Position(3, 3), Position(4, 3)};
    int num_fake_walls = 2;
    Position arr_fake_walls[] = {Position(1, 2), Position(4, 2)};

    Map * map = new Map(5, 5, num_walls, arr_walls, num_fake_walls, arr_fake_walls);

    Watson *watson = new Watson(1, "LUR", Position(0, 1), map, 100, 250);
    cout << watson->str() << endl;

    delete map;
    delete watson;
}
void tc_1138() {
    cout << "----- Testcase 138 -----" << endl;
    int num_walls = 14;
    Position arr_walls[] = {Position(1, 0), Position(1, 1), Position(1, 2), Position(1, 4), Position(1, 5), Position(1, 6), Position(3, 2), Position(3, 4), Position(5, 0), Position(5, 1), Position(5, 2), Position(5, 4), Position(5, 5), Position(5, 6)};
    int num_fake_walls = 3;
    Position arr_fake_walls[] = {Position(1, 3), Position(2, 3), Position(5, 3)};

    Map * map = new Map(7, 7, num_walls, arr_walls, num_fake_walls, arr_fake_walls);

    Sherlock * sherlock = new Sherlock(1, "URL", Position(0, 0), map, 100, 250);
    Watson * watson = new Watson(2, "URL", Position(0, 6), map, 100, 250);
    Criminal * criminal = new Criminal(0, Position(3, 3), map, sherlock, watson);
    cout << criminal->getNextPosition().str() << endl;

    delete map;
    delete sherlock;
    delete watson;
    delete criminal;
}
void tc_1143() {
    cout << "----- Testcase 143 -----" << endl;
    int num_walls = 14;
    Position arr_walls[] = {Position(1, 0), Position(1, 1), Position(1, 2), Position(1, 4), Position(1, 5), Position(1, 6), Position(3, 2), Position(3, 4), Position(5, 0), Position(5, 1), Position(5, 2), Position(5, 4), Position(5, 5), Position(5, 6)};
    int num_fake_walls = 3;
    Position arr_fake_walls[] = {Position(1, 3), Position(2, 3), Position(5, 3)};

    Map * map = new Map(7, 7, num_walls, arr_walls, num_fake_walls, arr_fake_walls);

    Sherlock * sherlock = new Sherlock(1, "URL", Position(0, 0), map, 100, 250);
    Watson * watson = new Watson(2, "URL", Position(0, 6), map, 100, 250);
    Criminal * criminal = new Criminal(0, Position(3, 3), map, sherlock, watson);
    criminal->move();
    cout << criminal->str();

    delete map;
    delete sherlock;
    delete watson;
    delete criminal;
}
void tc_1153() {
    cout << "----- Testcase 153 -----" << endl;
    int num_walls = 14;
    Position arr_walls[] = {Position(1, 0), Position(1, 1), Position(1, 2), Position(1, 4), Position(1, 5), Position(1, 6), Position(3, 2), Position(3, 4), Position(5, 0), Position(5, 1), Position(5, 2), Position(5, 4), Position(5, 5), Position(5, 6)};
    int num_fake_walls = 3;
    Position arr_fake_walls[] = {Position(1, 3), Position(2, 3), Position(5, 3)};

    Map * map = new Map(7, 7, num_walls, arr_walls, num_fake_walls, arr_fake_walls);

    Sherlock * sherlock = new Sherlock(1, "URL", Position(0, 0), map, 100, 250);
    Watson * watson = new Watson(2, "URL", Position(0, 6), map, 100, 250);
    Criminal * criminal = new Criminal(0, Position(3, 3), map, sherlock, watson);
    cout << criminal->str() << endl;

    delete map;
    delete sherlock;
    delete watson;
    delete criminal;
}
void tc_1159() {
    cout << "----- Testcase 159 -----" << endl;
    int num_walls = 14;
    Position arr_walls[] = {Position(1, 0), Position(1, 1), Position(1, 2), Position(1, 4), Position(1, 5), Position(1, 6), Position(3, 2), Position(3, 4), Position(5, 0), Position(5, 1), Position(5, 2), Position(5, 4), Position(5, 5), Position(5, 6)};
    int num_fake_walls = 3;
    Position arr_fake_walls[] = {Position(1, 3), Position(2, 3), Position(5, 3)};

    Map * map = new Map(7, 7, num_walls, arr_walls, num_fake_walls, arr_fake_walls);

    Sherlock * sherlock = new Sherlock(1, "LU", Position(0, 0), map, 100, 250);
    Watson * watson = new Watson(2, "LU", Position(0, 6), map, 100, 250);
    Criminal * criminal = new Criminal(0, Position(3, 3), map, sherlock, watson);
    cout << criminal->str() << endl;

    delete map;
    delete sherlock;
    delete watson;
    delete criminal;
}
void tc_1167() {
    cout << "----- Testcase 167 -----" << endl;

    ArrayMovingObject * arr_mv_objs = new ArrayMovingObject(5);
    cout << arr_mv_objs->str() << endl;
    delete arr_mv_objs;
}
bool tc_1171() {
    cout << "----- Testcase 171 -----" << endl;

    int num_walls = 3;
    Position arr_walls[] = {Position(1, 2), Position(2, 3), Position(3, 4)};
    int num_fake_walls = 1;
    Position arr_fake_walls[] = {Position(2, 0)};

    Map * map = new Map(10, 10, num_walls, arr_walls, num_fake_walls, arr_fake_walls);

    Sherlock * sherlock = new Sherlock(1, "RUU", Position(1, 3), map, 250, 450);
    

    ArrayMovingObject * arr_mv_objs = new ArrayMovingObject(3);
    bool result = arr_mv_objs->add(sherlock);
    delete arr_mv_objs;
    delete sherlock;
    delete map;

    return result;
}
void tc_1181() {
    cout << "----- Testcase 181 -----" << endl;

    Configuration * config = new Configuration("181.txt"); // 168
    cout << config->str() << endl;
    delete config;
}
void tc_1185() {
    cout << "----- Testcase 185 -----" << endl;

    Configuration * config = new Configuration("185.txt");
    cout << config->str() << endl;
    delete config;
}
void tc_1199() {
    cout << "----- Testcase 199 -----" << endl;
    int num_walls = 5;
    Position arr_walls[] = {Position(1, 2), Position(2, 3), Position(3, 4), Position(3,2), Position(4,3)};
    int num_fake_walls = 1;
    Position arr_fake_walls[] = {Position(2, 0)};

    Map * map = new Map(10, 10, num_walls, arr_walls, num_fake_walls, arr_fake_walls);

    Sherlock * sherlock = new Sherlock(1, "RUU", Position(1, 3), map, 250, 450);
    Watson * watson = new Watson(2, "LU", Position(2, 1), map, 300, 350);
    Criminal * criminal = new Criminal(0, Position(7, 9), map, sherlock, watson);
    RobotS * robotS = new RobotS(3, Position(3,3), map, criminal, sherlock);
    int distance_old = robotS->getDistance();

    Position new_position = robotS->getNextPosition();
    RobotS * robotS_new = new RobotS(4, new_position, map, criminal, sherlock);
    int distance_new = robotS->getDistance();

    if(distance_old - distance_new >= 0) cout << "Can get close";
    else cout << "Can not get close";

    delete robotS;
    delete robotS_new;
    delete sherlock;
    delete watson;
    delete criminal;
    delete map;
}
void tc_1211() {
    cout << "----- Testcase 211 -----" << endl;
    int num_walls = 4;
    Position arr_walls[] = {Position(7, 0), Position(6, 1), Position(8, 1), Position(7,2)};
    int num_fake_walls = 1;
    Position arr_fake_walls[] = {Position(2, 0)};

    Map * map = new Map(10, 10, num_walls, arr_walls, num_fake_walls, arr_fake_walls);

    Sherlock * sherlock = new Sherlock(1, "RUU", Position(1, 3), map, 250, 450);
    Watson * watson = new Watson(2, "LU", Position(2, 1), map, 300, 350);

    Criminal * criminal = new Criminal(0, Position(7, 9), map, sherlock, watson);
    RobotW * robotW = new RobotW(3, Position(7,1), map, criminal, watson);
    int distance_old = robotW->getDistance();

    Position new_position = robotW->getNextPosition();
    RobotW * robotW_new = new RobotW(4, new_position, map, criminal, watson);
    int distance_new = robotW->getDistance();

    if(distance_old - distance_new >= 0) cout << "Can get close";
    else cout << "Can not get close";

    delete robotW;
    delete robotW_new;
    delete sherlock;
    delete watson;
    delete criminal;
    delete map;
}
void tc_1223() {
    cout << "----- Testcase 223 -----" << endl;
    int num_walls = 4;
    Position arr_walls[] = {Position(7, 0), Position(6, 1), Position(8, 1), Position(7,2)};
    int num_fake_walls = 1;
    Position arr_fake_walls[] = {Position(2, 0)};

    Map * map = new Map(10, 10, num_walls, arr_walls, num_fake_walls, arr_fake_walls);

    Sherlock * sherlock = new Sherlock(1, "RUU", Position(1, 3), map, 250, 450);
    Watson * watson = new Watson(2, "LU", Position(2, 1), map, 300, 350);

    Criminal * criminal = new Criminal(0, Position(7, 9), map, sherlock, watson);
    RobotSW * robotSW = new RobotSW(3, Position(7,1), map, criminal, sherlock, watson);
    int distance_old = robotSW->getDistance();

    Position new_position = robotSW->getNextPosition();
    RobotSW * robotSW_new = new RobotSW(4, new_position, map, criminal, sherlock, watson);
    int distance_new = robotSW->getDistance();

    if(distance_old - distance_new >= 0) cout << "Can get close";
    else cout << "Can not get close";

    delete robotSW;
    delete robotSW_new;
    delete sherlock;
    delete watson;
    delete criminal;
    delete map;
}
void tc_1235() {
    cout << "----- Testcase 235 -----" << endl;
    int num_walls = 4;
    Position arr_walls[] = {Position(7, 0), Position(6, 1), Position(8, 1), Position(7,2)};
    int num_fake_walls = 1;
    Position arr_fake_walls[] = {Position(2, 0)};

    Map * map = new Map(10, 10, num_walls, arr_walls, num_fake_walls, arr_fake_walls);

    Sherlock * sherlock = new Sherlock(1, "RUU", Position(1, 3), map, 250, 450);
    Watson * watson = new Watson(2, "LU", Position(2, 1), map, 300, 350);

    Criminal * criminal = new Criminal(0, Position(7, 9), map, sherlock, watson);
    RobotC * robotC = new RobotC(3, Position(7,1), map, criminal);

    cout << "First position: " << robotC->getCurrentPosition().str() << endl;
    Position new_position = robotC->getNextPosition();
    RobotC * robotC_new = new RobotC(4, new_position, map, criminal);

    cout << "Next position: " << robotC_new->getCurrentPosition().str() << endl;

    delete robotC;
    delete robotC_new;
    delete sherlock;
    delete watson;
    delete criminal;
    delete map;
}
void tc_1247() {
    cout << "----- Testcase 247 -----" << endl;
    int num_walls = 3;
    Position arr_walls[] = {Position(1, 2), Position(2, 2), Position(3, 2)};
    int num_fake_walls = 1;
    Position arr_fake_walls[] = {Position(2, 0)};

    Map * map = new Map(10, 10, num_walls, arr_walls, num_fake_walls, arr_fake_walls);

    Sherlock * sherlock = new Sherlock(1, "RUU", Position(2, 4), map, 250, 450);
    Watson * watson = new Watson(2, "LU", Position(2, 5), map, 300, 350);

    Criminal * criminal = new Criminal(0, Position(5, 6), map, sherlock, watson);

    ArrayMovingObject * arr_mv_objs = new ArrayMovingObject(10);
    arr_mv_objs->add(sherlock);
    arr_mv_objs->add(watson);
    arr_mv_objs->add(criminal);

    for(int i = 0; i < 2; i++) {
        criminal->move();
    }

    cout << arr_mv_objs->str() << endl;

    delete sherlock;
    delete watson;
    delete criminal;
    delete arr_mv_objs;
    delete map;
}
void tc_1297() {
    cout << "----- Testcase 297 -----" << endl;
    int num_walls = 4;
    Position arr_walls[] = {Position(1, 2), Position(2, 3), Position(3, 4), Position(4,5)};
    int num_fake_walls = 2;
    Position arr_fake_walls[] = {Position(2, 0), Position(4,0)};

    Map * map = new Map(10, 10, num_walls, arr_walls, num_fake_walls, arr_fake_walls);

    Sherlock * sherlock = new Sherlock(1, "RUU", Position(1, 3), map, 150, 450);

    BaseBag * sherlockBag = new SherlockBag(sherlock);

    cout << sherlockBag->str() << endl;

    delete map;
    delete sherlock;
    delete sherlockBag;
}
void tc_1323() {
    cout << "----- Testcase 323 -----" << endl;
    int num_walls = 4;
    Position arr_walls[] = {Position(1, 2), Position(2, 3), Position(3, 4), Position(4,5)};
    int num_fake_walls = 2;
    Position arr_fake_walls[] = {Position(2, 0), Position(4,0)};

    Map * map = new Map(10, 10, num_walls, arr_walls, num_fake_walls, arr_fake_walls);

    Watson * watson = new Watson(1, "RUU", Position(1, 3), map, 150, 450);

    BaseBag * watsonBag = new WatsonBag(watson);

    cout << watsonBag->str() << endl;

    delete map;
    delete watson;
}


class TestStudyInPink {
public:
    TestStudyInPink() = default;
    
    static void tc_1001() {
        ::tc_1001();
    }
    static void tc_1004() {
        ::tc_1004();
    }
    static void tc_1009() {
        ::tc_1009();
    }
    static void tc_1014() {
        ::tc_1014();
    }
    static void tc_1015() {
        ::tc_1015();
    }
    static void tc_1016() {
        ::tc_1016();
    }
    static void tc_1017() {
        ::tc_1017();
    }
    static void tc_1043() {
        cout << "----- Testcase 43 -----" << endl;
        int num_walls = 0;
        Position arr_walls[] = {};
        int num_fake_walls = 1;
        Position arr_fake_walls[] = {Position(5, 5)};
        Map * map = new Map(10, 10, num_walls, arr_walls, num_fake_walls, arr_fake_walls);
        for (int i = 0; i < map->num_rows; i++) {
            for (int j = 0; j < map->num_cols; j++) {
                cout << map->map[i][j]->getType();
                if (map->map[i][j]->getType() == FAKE_WALL) {
                    FakeWall *fakewall = dynamic_cast<FakeWall*>(map->map[i][j]);
                    cout << fakewall->getReqExp();
                }
                cout << "-";
            }
            cout << endl;    
        }
        delete map;
    }
    static void tc_1057() {
        ::tc_1057();
    }
    static void tc_1062() {
        ::tc_1062();
    }
    static void tc_1063() {
        ::tc_1063();
    }
    static void tc_1066() {
        ::tc_1066();
    }
    static void tc_1067() {
        ::tc_1067();
    }
    static void tc_1070() {
        ::tc_1070;
    }
    static void tc_1094() {
        ::tc_1094();
    }
    static void tc_1099() {
        ::tc_1099;
    }
    static void tc_1109() {
        ::tc_1109;
    }
    static void tc_1114() {
        ::tc_1114;
    }
    static void tc_1120() {
        ::tc_1120;
    }
    static void tc_1132() {
        ::tc_1132;
    }
    static void tc_1138() {
        ::tc_1138;
    }
    static void tc_1143() {
        ::tc_1143;
    }
    static void tc_1153() {
        ::tc_1153;
    }
    static void tc_1159() {
        ::tc_1159;
    }
    static void tc_1167() {
        ::tc_1167;
    }
    static void tc_1171() {
        ::tc_1171;
    }
    static void tc_1181() {
        ::tc_1181;
    }
    static void tc_1185() {
        ::tc_1185;
    }
    static void tc_1199() {
        ::tc_1199;
    }
    static void tc_1211() {
        ::tc_1211;
    }
    static void tc_1223() {
        ::tc_1223;
    }
    static void tc_1235() {
        ::tc_1235;
    }
    static void tc_1247() {
        ::tc_1247;
    }
    static void tc_1278() {
        cout << "----- Testcase 278 -----" << endl;
        int num_walls = 4;
        Position arr_walls[] = {Position(1, 2), Position(2, 3), Position(3, 4), Position(4,5)};
        int num_fake_walls = 2;
        Position arr_fake_walls[] = {Position(2, 0), Position(4,0)};
    
        Map * map = new Map(10, 10, num_walls, arr_walls, num_fake_walls, arr_fake_walls);
            
        Sherlock * sherlock = new Sherlock(1, "RUU", Position(1, 3), map, 250, 450);
    
        Watson * watson = new Watson(2, "LU", Position(2, 1), map, 300, 350);
    
        Criminal * criminal = new Criminal(0, Position(7, 9), map, sherlock, watson);
    
        BaseItem * excemptionCard = new ExcemptionCard();
    
        cout << excemptionCard->canUse(criminal, NULL);
    
        delete map;
        delete sherlock;
        delete watson;
        delete criminal;
        delete excemptionCard;
    }
    
    static void tc_1297() {
        ::tc_1297;
    }
    static void tc_1323() {
        ::tc_1323;
    }
    static void tc_1286() {
        cout << "----- Testcase 286 -----" << endl;
        int num_walls = 4;
        Position arr_walls[] = {Position(1, 2), Position(2, 3), Position(3, 4), Position(4,5)};
        int num_fake_walls = 2;
        Position arr_fake_walls[] = {Position(2, 0), Position(4,0)};
    
        Map * map = new Map(10, 10, num_walls, arr_walls, num_fake_walls, arr_fake_walls);
            
        Sherlock * sherlock = new Sherlock(1, "RUU", Position(1, 3), map, 250, 450);
    
        Watson * watson = new Watson(2, "LU", Position(2, 1), map, 300, 350);
    
        Criminal * criminal = new Criminal(0, Position(7, 9), map, sherlock, watson);
    
        BaseItem * passingCard = new PassingCard("all");
    
        cout << passingCard->canUse(criminal, NULL);
    
        delete map;
        delete sherlock;
        delete watson;
        delete criminal;
        delete passingCard;
    }
    static void tc_1194() {
        cout << "----- Testcase 194 -----" << endl;

        Configuration * config = new Configuration("185.txt");
        cout << "Map Num Rows: " << config->map_num_rows << endl;
        cout << "Map Num Cols: " << config->map_num_cols << endl;
        cout << "Max Num Moving Objects: " << config->max_num_moving_objects << endl;
        cout << "Num Wall: " << config->num_walls << endl;
        if (config->num_walls != 0) {
            cout << "Array Wall: " << config->arr_walls->str() << endl;
        }
        cout << "Num Fake Wall: " << config->num_fake_walls << endl;
        if (config->num_fake_walls != 0) {
            cout << "Array Fake Wall: " << config->arr_fake_walls->str() << endl;
        }
        cout << "Shelock Moving Rule: " << config->sherlock_moving_rule << endl;
        cout << "Sherlock init position: " << config->sherlock_init_pos.str() << endl;
        cout << "Sherlock init hp: " << config->sherlock_init_hp << endl;
        cout << "Sherlock init exp: " << config->sherlock_init_exp << endl;
        cout << "Watson Moving Rule: " << config->watson_moving_rule << endl;
        cout << "Watson init position: " << config->watson_init_pos.str() << endl;
        cout << "Watson init hp: " << config->watson_init_hp << endl;
        cout << "Watson init exp: " << config->watson_init_exp << endl;
        cout << "Criminal init position: " << config->criminal_init_pos.str() << endl;
        cout << "Number step: " << config->num_steps << endl;
        delete config;
    }
    static void tc_1253() {
        cout << "----- Testcase 253 -----" << endl;
        BaseItem * magicBook = new MagicBook();
        
        delete magicBook;
    }
    static void tc_1354() {
        cout << "----- Testcase 354 -----" << endl;
        int num_walls = 4;
        Position arr_walls[] = {Position(1, 2), Position(2, 3), Position(3, 4), Position(4,5)};
        int num_fake_walls = 2;
        Position arr_fake_walls[] = {Position(2, 0), Position(4,0)};
    
        Map * map = new Map(10, 10, num_walls, arr_walls, num_fake_walls, arr_fake_walls);
    
        Sherlock * sherlock = new Sherlock(1, "RUU", Position(1, 3), map, 290, 520);
    
        Watson * watson = new Watson(2, "LU", Position(2, 1), map, 320, 350);
    
        Criminal * criminal = new Criminal(0, Position(7, 9), map, sherlock, watson);
    
        RobotC * robotC = new RobotC(3, Position(1, 4), map, criminal);
    
        sherlock->move();
    
        cout << sherlock->str() << endl;
    
        delete map;
        delete sherlock;
        delete watson;
        delete criminal;
        delete robotC;
    }
    static void satc_01() {
        cout << "----- Sample Testcase 01 -----" << endl;
        int num_walls = 3;
        Position arr_walls[] = {Position(1, 2), Position(2, 3), Position(3, 4)};
        int num_fake_walls = 1;
        Position arr_fake_walls[] = {Position(2, 0)};

        Map * map = new Map(10, 10, num_walls, arr_walls, num_fake_walls, arr_fake_walls);
        
        Sherlock * sherlock = new Sherlock(1, "RUU", Position(1, 3), map, 250, 450);
        cout << sherlock->str() << endl;

        Watson * watson = new Watson(2, "LU", Position(2, 1), map, 300, 350);
        cout << watson->str() << endl;

        Criminal * criminal = new Criminal(0, Position(7, 9), map, sherlock, watson);
        cout << criminal->str() << endl;

        cout << "* Sherlock makes a move" << endl;
        sherlock->move();
        cout << sherlock->str() << endl;

        cout << "* Watson makes a move" << endl;
        watson->move();
        cout << watson->str() << endl;

        cout << "* Criminal makes a move" << endl;
        criminal->move();
        cout << criminal->str() << endl;

        ArrayMovingObject * arr_mv_objs = new ArrayMovingObject(10);
        arr_mv_objs->add(criminal);
        arr_mv_objs->add(sherlock);
        arr_mv_objs->add(watson);
        cout << arr_mv_objs->str() << endl;
        
        delete arr_mv_objs;
        delete sherlock;
        delete watson;
        delete criminal;
        delete map;
    }

    static void satc_02() {
        cout << "----- Sample Testcase 02 -----" << endl;
        const string & filename = "185.txt"; 
        Configuration * config = new Configuration(filename);
        cout << config->str() << endl;
        delete config;
    }

    static void satc_03() {
        // Sample test for RobotC
        cout << "----- Sample Testcase 03 -----" << endl;
        int num_walls = 3;
        Position arr_walls[] = {Position(1, 2), Position(2, 3), Position(3, 4)};
        int num_fake_walls = 1;
        Position arr_fake_walls[] = {Position(2, 0)};

        Map * map = new Map(10, 10, num_walls, arr_walls, num_fake_walls, arr_fake_walls);

        Sherlock * sherlock = new Sherlock(1, "RUU", Position(1, 3), map, 250, 450);
        Watson * watson = new Watson(2, "LU", Position(2, 1), map, 300, 350);
        Criminal * criminal = new Criminal(0, Position(7, 9), map, sherlock, watson);
        criminal->move();

        RobotC * robotC = new RobotC(3, Position(7,9), map, criminal);
        cout << "Current position of Criminal: " << criminal->str() << endl;
        cout << "Criminal makes a move" << endl;
        criminal->move();
        cout << "New position of Criminal: " << criminal->str() << endl;

        cout << "Current position of robotC: " << robotC->str() << endl;
        cout << "RobotC makes a move" << endl;
        robotC->move();
        cout << "New position of robotC: " << robotC->str() << endl;

        delete sherlock;
        delete watson;
        delete criminal;
        delete map;
        delete robotC;
    }
};
void test() {
    TestStudyInPink::tc_1001();
    TestStudyInPink::tc_1004();
    TestStudyInPink::tc_1009();
    TestStudyInPink::tc_1014();
    TestStudyInPink::tc_1015();
    TestStudyInPink::tc_1016();
    TestStudyInPink::tc_1017();
    TestStudyInPink::tc_1043();
    TestStudyInPink::tc_1057();
    TestStudyInPink::tc_1062();
    TestStudyInPink::tc_1063();
    TestStudyInPink::tc_1066();
    TestStudyInPink::tc_1067();
    TestStudyInPink::tc_1070();
    TestStudyInPink::tc_1094();
    TestStudyInPink::tc_1099();
    TestStudyInPink::tc_1109();
    TestStudyInPink::tc_1114();
    TestStudyInPink::tc_1120();
    TestStudyInPink::tc_1132();
    TestStudyInPink::tc_1138();
    TestStudyInPink::tc_1143();
    TestStudyInPink::tc_1153();
    TestStudyInPink::tc_1159();
    TestStudyInPink::tc_1167();
    TestStudyInPink::tc_1171();
    TestStudyInPink::tc_1181();
    TestStudyInPink::tc_1185();
    TestStudyInPink::tc_1199();
    TestStudyInPink::tc_1211();
    TestStudyInPink::tc_1223();
    TestStudyInPink::tc_1235();
    TestStudyInPink::tc_1247();
    TestStudyInPink::tc_1278();
}
int main(int argc, const char * argv[]) {
    test();
    return 0;
}