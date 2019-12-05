#-*- coding:utf-8 -*-
import numpy as np


equal = lambda x,y : 0 if (x == y) else 1
def get_edit_path(ptr_matrix):
    '''
    通过ptr操作记录的矩阵来还原编辑过程
    '''
    path = []
    sub_num = 0
    ins_num = 0
    del_num = 0
    start = ptr_matrix[ptr_matrix.shape[0] - 1, ptr_matrix.shape[1] - 1]
    while (isinstance(start, int) != True):
        path.append(start[0])
        start = ptr_matrix[start[1][0], start[1][1]]
    path.reverse()
    for flag in path:
        if flag == 'sub':
            sub_num = sub_num + 1
        elif flag == 'deletion':
            del_num = del_num + 1
        elif flag == 'insertion':
            ins_num = ins_num + 1
    #print('sub', sub_num)
    #print('del', del_num)
    #print('ins', ins_num)
    #return path 
    # sub_num, del_num, ins_num
    return sub_num + del_num

def get_edit_distance(hyp, ref):
    '''
    hyp: 输入串
    ref: 原始字符串
    使用动态规划的方法实现编辑距离,
    insert、deletion、sub等错误描述的是由输入串到原始串的操作 比如 hyp: 'abbcd', ref: 'ac', 三个insert错误。
    '''
    len_hyp = len(hyp)
    len_ref = len(ref)
    
    # 矩阵的横轴为输入串，纵轴为原始串，矩阵用来保存当前的最优编辑距离是多少
    edit_matrix = np.zeros((len_ref + 1, len_hyp + 1), dtype = int)
    edit_matrix[0,:] = [i for i in range(len_hyp + 1)]
    edit_matrix[:,0] = [i for i in range(len_ref + 1)]

    # 记录每一步的路径
    ptr_matrix = np.zeros((len_ref + 1, len_hyp + 1), dtype = tuple)
    ptr_matrix[0, 1:] = [('insertion', (0, i - 1)) for i in range(1, len_hyp + 1)]
    ptr_matrix[1:, 0] = [('deletion', (i - 1, 0)) for i in range(1, len_ref + 1)]

    for i in range(1, edit_matrix.shape[0]):
        for j in range(1, edit_matrix.shape[1]):

            # 是否有替换错误
            is_equal = equal(ref[i - 1], hyp[j - 1])
            sub = edit_matrix[i - 1, j - 1] + is_equal

            # 删除错误 (j 少了一个)
            deletion = edit_matrix[i - 1, j] + 1

            # 插入错误 (j 多插入了一个)
            insertion = edit_matrix[i, j - 1] + 1

            min_edit = min(sub, insertion, deletion)

            # 记录路径
            if (sub == min_edit):
                if (is_equal):
                    ptr_matrix[i, j] = ('sub', (i - 1, j - 1))
                else:
                    ptr_matrix[i, j] = ('', (i - 1, j - 1))
            elif (deletion == min_edit):
                ptr_matrix[i, j] = ('deletion', (i - 1, j))
            elif (insertion == min_edit):
                ptr_matrix[i, j] = ('insertion', (i, j - 1))
            else:
                print ('error')

            edit_matrix[i, j] = min_edit

    # print (ptr_matrix)
    #print (get_edit_path(ptr_matrix))
    return edit_matrix[len_ref, len_hyp], get_edit_path(ptr_matrix)#, edit_matrix

# ttt = get_edit_distance('今天天气嗯嗯非常不措', '今天天气非常的不错')
# print ('edit distance:', ttt[0])
# print ('paths: ', ttt[1])
#display (pd.DataFrame(ttt[2], columns = [i for i in '#今天天气嗯嗯非常不措'], index = [i for i in '#今天天气非常的不错']))