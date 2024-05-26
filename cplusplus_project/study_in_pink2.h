/*
 * Ho Chi Minh City University of Technology
 * Faculty of Computer Science and Engineering
 * Initial code for Assignment 1
 * Programming Fundamentals Spring 2023
 * Author: Vu Van Tien
 * Date: 02.02.2023
 */

// The library here is concretely set, students are not allowed to include any other libraries.
#ifndef _H_STUDY_IN_PINK_2_H_
#define _H_STUDY_IN_PINK_2_H_

#include "main.h"

////////////////////////////////////////////////////////////////////////
/// STUDENT'S ANSWER BEGINS HERE
/// Complete the following functions
/// DO NOT modify any parameters in the functions.
////////////////////////////////////////////////////////////////////////

// Forward declaration
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
        int getExp() { return this->exp; }
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
        int map_num_rows; // so hang cua ban do  
		int map_num_cols; // so cot cua ban do 
		int map_num_moving_objects; // so luong phan tu toi da cua mang cac doi tuong di chuyen 
		int num_walls;  // so luong doi tuong wall 
		Position* arr_walls; // danh sach cac vi tri cua cac Wall
		int num_fake_walls; // so luong doi tuong fakewall
		Position* arr_fake_walls; // danh sach cac vi tri cua cac FakeWall 
		string sherlock_moving_rule; // moving_rule cua Sherlock
		Position sherlock_init_pos; // vi tri ban dau cua Sherlock
		int sherlock_init_hp; 
		int sherlock_init_exp; 
		string watson_moving_rule; // moving rule cua Watson
		Position watson_init_pos; // vi tri ban dau cua watson 
		int watson_init_hp; 
		int watson_init_exp; 
		Position criminal_init_pos; // vi tri ban dau cua ten toi pham 
		int num_steps; // so vong lap ma chuong trinh se thuc hien

    public:
		Configuration(const string & filepath);
		~Configuration();
        string str() const ;
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
////////////////////////////////////////////////
/// END OF STUDENT'S ANSWER
////////////////////////////////////////////////
#endif /* _H_STUDY_IN_PINK_2_H_ */
