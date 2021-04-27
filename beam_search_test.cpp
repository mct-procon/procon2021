//5*5くらいなら解ける、枝狩りもなにもしてないので計算量がやばい
#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <queue>
#include <string>
#include <math.h>
using namespace std;

const int inf=2e9;
const int beam_width=200;
const double time_limit=5.0;

double start_time;
int height, width;
int select_limit, select_cost, change_cost;

//状態
struct status{
    vector<vector<int>> place;
    int selecting_piece;
    int select_times;
    int change_times;
    int eval_val;
    vector<pair<int,int>> operations;
    //first=0のとき選択、secondが選択したピース
    //first=1のとき交換、secondが向きに対応 {0,1,2,3}={D,R,U,L}
    bool operator<(const status &another) const {
        return eval_val < another.eval_val;
    };
};

//xとyのマンハッタン距離
int dist(int x, int y){
    int dx=abs(x/width-y/width);
    int dy=abs(x%width-y%width);
    return min(dx,height-dx)+min(dy,width-dy);
}

//盤面評価（適当）
int evaluation(status &s){
    int val=0;
    for(int i=0;i<height;i++){
        for(int j=0;j<width;j++){
            val-=10000*dist(s.place[i][j],i*width+j);
        }
    }
    //ピースの一致が最優先
    val-=s.select_times*select_cost;
    val-=s.change_times*change_cost;
    return val;
}

//終了判定
bool end_check(status &s){
    bool flag=1;
    for(int i=0;i<height;i++){
        for(int j=0;j<width;j++){
            if(s.place[i][j]!=i*width+j){
                flag=0;
                break;
            }
        }
        if(!flag)
            break;
    }
    return flag;
}

status solve(status init){
    //ビームサーチ
    //汚いので後で直す
    status old_status, new_status;
    status answer=init;
    priority_queue<status> que;
    que.push(init);
    for(int depth=0;true;depth++){
        priority_queue<status> next_que;
        for(int k=0;k<beam_width;k++){
            if(que.empty()){
                break;
            }
            old_status=que.top();
            que.pop();
            if(end_check(old_status))//完成してたらcontinue;
                continue;
            int selecting=old_status.selecting_piece;
            for(int i=0;i<height;i++){
                for(int j=0;j<width;j++){
                    //下のピースと交換
                    new_status=old_status;
                    if(selecting==new_status.place[i][j]||selecting==new_status.place[(i+1)%height][j]){
                        //交換するピースのどちらかが選択中の場合
                        new_status.change_times++;
                        if(selecting==new_status.place[i][j])
                            new_status.operations.push_back({1,0});
                        else
                            new_status.operations.push_back({1,2});
                        swap(new_status.place[i][j],new_status.place[(i+1)%height][j]);
                        new_status.eval_val=evaluation(new_status);
                        next_que.push(new_status);
                    }
                    else{
                        //交換するピースがどちらも選択中でない場合
                        new_status.select_times++;
                        new_status.change_times++;
                        if(new_status.select_times>select_limit)//選択制限を超えていたらcontinue
                            continue;
                        swap(new_status.place[i][j],new_status.place[(i+1)%height][j]);
                        new_status.eval_val=evaluation(new_status);
                        //(i,j)を選択したことにする
                        new_status.operations.push_back({0,new_status.place[(i+1)%height][j]});
                        new_status.operations.push_back({1,0});
                        new_status.selecting_piece=new_status.place[(i+1)%height][j];
                        next_que.push(new_status);
                        new_status.operations.pop_back(); new_status.operations.pop_back();
                        //((i+1)%height,j)を選択したことにする
                        new_status.operations.push_back({0,new_status.place[i][j]});
                        new_status.operations.push_back({1,2});
                        new_status.selecting_piece=new_status.place[i][j];
                        next_que.push(new_status);
                    }
                    //右のピースと交換
                    new_status=old_status;
                    if(selecting==new_status.place[i][j]||selecting==new_status.place[i][(j+1)%width]){
                        //交換するピースのどちらかが選択中の場合
                        new_status.change_times++;
                        if(selecting==new_status.place[i][j])
                            new_status.operations.push_back({1,1});
                        else
                            new_status.operations.push_back({1,3});
                        swap(new_status.place[i][j],new_status.place[i][(j+1)%width]);
                        new_status.eval_val=evaluation(new_status);
                        next_que.push(new_status);
                    }
                    else{
                        //交換するピースがどちらも選択中でない場合
                        new_status.select_times++;
                        new_status.change_times++;
                        if(new_status.select_times>select_limit)//選択制限を超えていたらcontinue
                            continue;
                        swap(new_status.place[i][j],new_status.place[i][(j+1)%width]);
                        new_status.eval_val=evaluation(new_status);
                        //(i,j)を選択したことにする
                        new_status.operations.push_back({0,new_status.place[i][(j+1)%width]});
                        new_status.operations.push_back({1,1});
                        new_status.selecting_piece=new_status.place[i][(j+1)%width];
                        next_que.push(new_status);
                        new_status.operations.pop_back(); new_status.operations.pop_back();
                        //(i,(j+1)%width)を選択したことにする
                        new_status.operations.push_back({0,new_status.place[i][j]});
                        new_status.operations.push_back({1,3});
                        new_status.selecting_piece=new_status.place[i][j];
                        next_que.push(new_status);
                    }
                }
            }
        }
        if(next_que.empty()){
            break;
        }
        if(answer.eval_val<next_que.top().eval_val){
            //評価値が高ければanswerの更新
            answer=next_que.top();
        }
        if(depth%10==0){
            if((double)(clock()-start_time)/CLOCKS_PER_SEC/time_limit>=1.0){
                //制限時間超過
                break;
            }
        }
        que=next_que;
    }
    return answer;
}

int main(){
    start_time=clock();
    //入力
    cin >> height >> width;
    vector<vector<int>> init_place(height,vector<int>(width));
    /*
    for(int i=0;i<height;i++){
        for(int j=0;j<width;j++){
            cin >> init_place[i][j];
        }
    }
    */
    //ランダム生成
    vector<int> perm(height*width);
    iota(perm.begin(),perm.end(),0);
    random_shuffle(perm.begin(),perm.end());
    for(int i=0;i<height;i++){
        for(int j=0;j<width;j++){
            init_place[i][j]=perm[i*width+j];
            cout << perm[i*width+j] << " ";
        }
        cout << endl;
    }
    cin >> select_limit >> select_cost >> change_cost;
    status init=status({init_place,-1,0,0,0,vector<pair<int,int>>(0)});
    init.eval_val=evaluation(init);
    status answer=solve(init);
    //結果出力
    cout << "選択回数 : " << answer.select_times << endl;
    string dir="DRUL";
    for(auto p:answer.operations){
        if(p.first==0){
            cout << endl;
            cout << "選択 : " << p.second << endl;
        }
        else{
            cout << dir[p.second];
        }
    }
    cout << endl << endl;
    for(int i=0;i<height;i++){
        for(int j=0;j<width;j++){
            cout << answer.place[i][j] << " ";
        }
        cout << endl;
    }
    return 0;
}
