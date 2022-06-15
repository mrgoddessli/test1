#include <bits/stdc++.h>

#define rep(i, s, t) for(int i=s;i<=t;i++)
using namespace std;

/* 全局变量 */
int n;                   // 猪的数量
queue<char> allCards;    // 牌堆

struct Pig
{
/* 属性 */
    int index;            // 当前猪下标
    int hp = 4;           // 血量
    char type;            // 猪的类型: M, Z, F
    char jumpType = 0;    // 跳的类型: Z(忠), F(反), f(MP视角的类反), 0(空), 注意对MP来说针对F更优先于f
    bool arming = false;  // 有无装备
    list<char> cards;     // 手中的牌
/* 方法定义, 这里是完成本题必须的方法, 可以不必多定义了 */
    void jump()
    { jumpType = type == 'F' ? 'F' : 'Z'; } // 跳阵营
    bool isJumpItsFriend(Pig *pig)
    { return pig->type == 'F' ? jumpType == 'F' : jumpType == 'Z'; } // 我跳它朋友了吗?
    Pig *getNextPig();    // 获取存活的下家
    void addCards(int num);// 从牌堆拿牌放到自己手右
    bool del(char c);     // 删掉手里一张牌, 删成功则返回true
    void hurt(Pig *attacker);// 被攻击掉一点血
    bool cost(Pig *attacker, char c);// 被迫消耗手中一张牌, 消耗成功返回true并不减血, 否则减血, 减血到0尝试用桃, 无则死亡, 根据身份对杀者做后续处理
    bool useP();          // 主动使用桃, 满足使用条件就使用, 使用后返回true
    bool useK();          // 主动使用杀, 满足使用条件就使用, 使用后返回true
    bool useF();          // 主动使用决斗, 满足使用条件就使用, 使用后返回true
    bool useN();          // 主动使用南猪入侵, 满足使用条件就使用, 使用后返回true
    bool useW();          // 主动使用万箭齐发, 满足使用条件就使用, 使用后返回true
    bool findJ(Pig *attacker); // 被迫去求无懈可击, 求到则返回true
    bool useJ(Pig *pig);  // 帮某个猪用无懈可击, 用成功返回true
} ps[15];

/* 全局方法 */
bool isGameEnd()
{
    if (ps[0].hp <= 0) return true;
    rep(i, 1, n - 1)
    {
        if (ps[i].type == 'F' && ps[i].hp > 0)
            return false;
    }
    return true;
}

int cnt = 0;

void judgeGameEnd()
{
    if (isGameEnd())
    {
        printf(ps[0].hp > 0 ? "MP\n" : "FP\n");
        rep(i, 0, n - 1)
        {
            if (ps[i].hp <= 0)
            {
                printf("DEAD\n");
            }
            else
            {
                for (list<char>::iterator it = ps[i].cards.begin(); it != ps[i].cards.end(); it++)
                {
                    printf(it == ps[i].cards.end() ? "%c" : "%c ", *it);
                }
                printf("\n");
            }
        }
        exit(0);
    }
}

void solve()
{
    ps[0].jump();
    while (true)
    {
        rep(i, 0, n - 1)
        {
            if (ps[i].hp <= 0) continue;
            ps[i].addCards(2);
            bool usedK = false;
            for (list<char>::iterator it = ps[i].cards.begin(); it != ps[i].cards.end(); it++)
            {
                bool used = false;
                char c = *it;
                it = ps[i].cards.erase(it);
                switch (c)
                {
                    case 'P':
                        used = ps[i].useP();
                        break;
                    case 'K':
                        if (!usedK || ps[i].arming) used = usedK = ps[i].useK();
                        break;
                    case 'F':
                        used = ps[i].useF();
                        break;
                    case 'N':
                        used = ps[i].useN();
                        break;
                    case 'W':
                        used = ps[i].useW();
                        break;
                    case 'Z':
                        ps[i].arming = used = true;
                        break;
                }
                if (used) it = --ps[i].cards.begin();
                else it = ps[i].cards.insert(it, c);
                if (ps[i].hp <= 0) break;
            }
        }
    }
}

/* 主控方法 */
int main()
{
    // Input
    int m;
    scanf("%d%d", &n, &m);
    char s[5];
    rep(i, 0, n - 1)
    {
        ps[i].index = i;
        scanf("%s", s), ps[i].type = s[0];
        rep(j, 1, 4) scanf("%s", s), ps[i].cards.push_back(s[0]);
    }
    while (m--) scanf("%s", s), allCards.push(s[0]);
    // To solve
    solve();
}

Pig *Pig::getNextPig()
{
    int nxt = (index + 1) % n;
    while (ps[nxt].hp <= 0)
    {
        nxt = (nxt + 1) % n;
    }
    return &ps[nxt];
}

void Pig::addCards(int num)
{
    rep(i, 1, num)
    {
        cards.push_back(allCards.front());
        allCards.pop();
    }
}

bool Pig::useP()
{
    if (hp < 4)
    {
        hp++;
        return true;
    }
    return false;
}

void Pig::hurt(Pig *attacker)
{
    if (--hp == 0)
    {
        if (this->del('P'))
        {
            this->useP();
        }
        else
        {
            judgeGameEnd();
            switch (type)
            {
                case 'F':
                    attacker->addCards(3);
                    break;
                case 'Z':
                    if (attacker->type == 'M') attacker->cards.clear(), attacker->arming = false;
                    break;
            }
        }
    }
}

bool Pig::cost(Pig *attacker, char c)
{
    if (this->del(c))
    {
        return true;
    }
    hurt(attacker);
    return false;
}

bool Pig::findJ(Pig *attacker)
{
    Pig *nxt = attacker;
    do
    {
        // "找个好心的猪猪帮我挡刀"
        if (this->isJumpItsFriend(nxt) && nxt->del('J'))
        {
            nxt->jump();
            return !nxt->useJ(this);
        }
        nxt = nxt->getNextPig();
    } while (nxt != attacker);
    return false;
}

bool Pig::useJ(Pig *pig)
{
    for (Pig *nxt = getNextPig(); nxt != this; nxt = nxt->getNextPig())
    {
        // "套娃指找找有没有猪猪会阻止我帮别人挡刀"
        if (!this->isJumpItsFriend(nxt) && nxt->del('J'))
        {
            nxt->jump();
            return !pig->findJ(nxt);
        }
    }
    return false;
}

/**
 * what I need do
 * @return
 */
bool Pig::useK()        //使用杀
{
    // TODO: 补全代码

    char temp = this->type;             //当前猪猪的类型
    Pig *nex = this->getNextPig();      //能够到的那个猪猪

    if (temp == 'M')                    //主公
    {
        Pig *nxt = getNextPig();
        if (nxt->jumpType == 'F' or nxt->jumpType == 'f')       //跳反或者被认定为类反，使用杀
        {
            nxt->cost(this, 'D');           //对方需要出‘闪’
            return true;
        }
        else return false;                  //否则不使用杀
    }
    else
    {
        if (temp=='Z')              //如果是忠臣
        {
            if(nex->jumpType=='F')          //跳反为反贼
            {
                nex->cost(this, 'D');   //使用杀
                this->jump();                     //自己跳忠
                return true;
            }
            else
                return false;           //否则不适用杀
        }
        else if(temp=='F')              //如果是反贼
        {
            if(nex->jumpType=='Z')          //跳反为忠
            {
                nex->cost(this, 'D');       //使用杀，对方交闪
                this->jump();               //自己跳反
                return true;
            }
            else
                return false;           //否则不使用杀
        }
        return false;
    }
}

bool Pig::useF()        //决斗
{
    // TODO: 补全代码
    char temp = this->type;         //当前类型
    if (temp == 'Z')                //当前为忠臣
    {
        Pig *nxt = this->getNextPig();      //下一个猪猪

        while (nxt != this)         //遍历
        {
            if (nxt->jumpType == 'F')       //如果遇到跳反的反贼
            {
                this->jump();               //进行决斗，自己跳忠
                if (nxt->findJ(this))return true;   //如果有人无懈可击掉，直接返回

                for (int i = 0; i < 999; i++)
                {
                    if (!nxt->cost(this,'K'))       //否则进行决斗，对方先交杀
                    {
                        return true;            //没有杀则直接返回
                    }
                    if (!this->cost(nxt,'K'))       //自己交杀
                    {
                        return true;            //没杀则返回
                    }
                }
            }
            nxt = nxt->getNextPig();            //查找下一位猪猪
        }
        return false;           //所有人都不能决斗，则不使用决斗
    }
    else if (temp == 'M')           //主公
    {
        Pig *nxt = getNextPig();        //下一位猪猪
        bool havef = 0;                 //判断有无类反
        while (nxt != this)
        {
            if (nxt->jumpType == 'F')       //遇到跳反的反贼，直接决斗并返回
            {
                if (nxt->findJ(this))return true;   //有人无懈则直接返回

                for (int i = 0; i < 999; i++)
                {
                    if (!nxt->cost(this,'K'))   //对方先交杀
                    {
//                                nxt->hurt(this);
                        return true;
                    }
                    if (!this->cost(nxt,'K'))           //我方交杀
                    {
//                                this->hurt(nxt);
                        return true;
                    }
                }
            }
            else if (nxt->jumpType == 'f')havef = 1;        //遇到类反，记录下存在类反猪

            nxt = nxt->getNextPig();
        }
        nxt = this->getNextPig();       //更新为下一个
        if (havef)                      //无跳反猪但有类反猪
        {
            while (nxt != this)         //进行遍历
            {
                if (nxt->jumpType == 'f')   //遇到类反猪
                {
                    if (nxt->type == 'Z')           //类反猪是忠臣
                    {
                        nxt->hurt(this);        //对方直接扣血
                        return true;
                    }
                    else
                    {
                        for (int i = 0; i < 999; i++)
                        {
                            if (!nxt->cost(this,'K'))   //对方进行了回击
                            {
//                                nxt->hurt(this);
                                return true;
                            }
                            nxt->jump();        //说明对方是反贼，进行跳反
                            if (!this->cost(nxt,'K'))       //我方交杀
                            {

                                return true;
                            }
                        }
                    }
                }
                nxt = nxt->getNextPig();        //遍历
            }
        }
        else return false;      //否则不使用
    }
    else if (temp == 'F')       //反贼
    {
        jump();                 //直接跳反
        Pig *nxt = &ps[0];      //反贼只找主公干架

        if (nxt->findJ(this))return true;       //有无懈可击则直接返回

        for (int i = 0; i < 999; i++)
        {
            if (!nxt->cost(this,'K'))       //对方先交杀
            {
//                                nxt->hurt(this);
                return true;
            }
            if (!this->cost(nxt,'K'))               //我方交杀
            {
//                                this->hurt(nxt);
                return true;
            }
        }
    }

    return false;
}

bool Pig::useN()        //南蛮入侵
{
    for (Pig *nxt = getNextPig(); nxt != this; nxt = nxt->getNextPig())
    {
        // TODO: 补全代码
        if (nxt->findJ(this))continue;      //当前猪猪回合有人无懈可击则直接跳过

        if (!nxt->cost(this, 'K'))          //进行交杀
        {
            if (nxt->type == 'M')               //如果受伤的是主公
            {
                if (this->jumpType == 0)        //且自身没有跳
                    this->jumpType = 'f';       //认定为类反猪
            }
        }
    }
    return true;
}

bool Pig::useW()        //万箭齐发
{
    for (Pig *nxt = getNextPig(); nxt != this; nxt = nxt->getNextPig())
    {
        // TODO: 补全代码
        if (nxt->findJ(this))continue;      //当前猪猪回合有人无懈可击则直接跳过

        if (!nxt->cost(this, 'D'))          //进行交闪
        {
            if (nxt->type == 'M')               //如果受伤的是主公
            {
                if (this->jumpType == 0)        //且自身没有跳
                    this->jumpType = 'f';       //认定为类反猪
            }
        }
    }
    return true;
}

bool Pig::del(char c)       //删除牌
{
    // TODO: 补全代码
    for (auto i = this->cards.begin(); i != this->cards.end(); i++) //遍历
    {
        if (*i == c)            //找到
        {
            this->cards.erase(i);   //删除
            return true;        //成功
        }
    }
    return false;       //失败
}
