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

class MapElement
{
    friend class TestStudyInPink;

protected:
    ElementType type;

public:
    MapElement(ElementType in_type);
    virtual ~MapElement();
    virtual ElementType getType() const;
};

class Path : public MapElement
{
    friend class TestStudyInPink;

public:
    Path();
};

class Wall : public MapElement
{
    friend class TestStudyInPink;

public:
    Wall();
};

class FakeWall : public MapElement
{
    friend class TestStudyInPink;

private:
    int req_exp;

public:
    FakeWall(int in_req_exp);
    int getReqExp() const;
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

class Position
{
    friend class TestStudyInPink;

private:
    int r, c;

public:
    Position(int r = 0, int c = 0);
    Position(const string &str_pos);
    int getRow() const;
    int getCol() const;
    void setRow(int r);
    void setCol(int c);
    string str() const;
    bool isEqual(int in_r, int in_c) const;
    bool isEqual(Position pos) const;
    static const Position npos;
};

class MovingObject
{
    friend class TestStudyInPink;

protected:
    int index;
    Position pos;
    Map *map;
    string name;

public:
    MovingObject(int index, const Position pos, Map *map, const string &name = "");
    string getname();
    int getIndex() {return index;};
    virtual ~MovingObject();
    virtual Position getNextPosition() = 0;
    void setPosition(Position pos)
    {
        this->pos = pos;
    }

    Position getCurrentPosition() const;
    virtual void move() = 0;
    virtual string str() const = 0;
};
class Character : public MovingObject
{
    friend class TestStudyInPink;

protected:
    int hp;
    int exp;

public:
    Character(int index, const Position pos, Map *map, string character_type, int hp, int exp);
    int getExp();
    void setExp(int in_exp);
    int getHp();
    void setHp(int in_hp);
    virtual ~Character(){};
    virtual Position getNextPosition() override = 0;
    virtual void move() override = 0;
    virtual string str() const override = 0;
};

class Sherlock : public Character
{
    friend class TestStudyInPink;

private:
    // TODO
    string moving_rule;
    int move_index;

public:
    Sherlock(int index, const string &moving_rule, const Position &init_pos, Map *map, int init_hp, int init_exp);
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
    int move_index;

public:
    Watson(int index, const string &moving_rule, const Position &init_pos, Map *map, int init_hp, int init_exp);
    virtual Position getNextPosition() override;
    virtual void move() override;
    virtual string str() const override;
    int getExp() const { return exp; };
};

class Criminal : public Character
{
    friend class TestStudyInPink;

private:
    // TODO
    Sherlock *sherlock;
    Watson *watson;
    Position prevpos;

public:
    Criminal(int index, const Position &init_pos, Map *map, Sherlock *sherlock, Watson *watson);
    Position get_prevpos();
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
    ArrayMovingObject(int capacity);
    ~ArrayMovingObject();
    bool isFull() const;
    bool add(MovingObject *mv_obj);
    MovingObject *get(int index) const;
    int size() const; // return current number of elements in the array
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
