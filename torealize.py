'''
主要解决几个问题：
1.对于UI点击选择多种乐器的载入
2.如何实现多张乐谱的重叠演奏，具体实现需要通过Qt中的QTimer
3.如何实时为演奏添加乐器种类
'''
#一张乐谱写成一个对象，演奏时对多张乐谱进行重叠演奏
class singleplaying:
    def __init__(self,music_score,instrument):
        self.score_to_play=music_score
        self.instrument=instrument
    def singleplay(yinliang=-1,jiepai=8):
        score=self.score_to_play
        instru=self.instrument
        to_expand_score() #根据节拍将乐谱转化为相应节奏的乐谱，不足处用停顿补充
        for yinfu in score: #每个停顿也作为一个音符
            #根据节拍进行
            single=instrument_respective(instru) #对每个音符创建对应乐器类的对象
            *instrument() p=single #将对应乐器类转化为统一的乐器大类指针，方便调用
            p.play(yinfu) #调用库函数演奏

scores_total=[] #实际实现是使用map，方便演奏是实时删除，添加新乐谱（乐器）
#UI勾选并确认后调用该函数
def load_score(new_scores):
    #new_scores作为一个list
    for item in new_scores:
        scores_total.append(item)

#终止后清空scores
def stop_to_play():
    scores_total.flush()

#存储用户创建的乐谱组合
def download():

#UI访问存储记录时调用
def display_download():

#点击某个历史乐谱时调用，将其加入现有乐谱体系
def rep_layout():
    #复现

#点击演奏时调用
def paly():
    load(music_score)
    for in_score in music_score:
        to_play(in_score)

