import numpy as np
import cv2
import matplotlib.pyplot as plt
#画像読み込み
input_img = cv2.imread('../images/lenna.jpg', 0)
#create_histogram
img_hist = cv2.calcHist([input_img], channels = [0], mask = None, histSize = [256], ranges = [0, 256])

plt.plot(img_hist), plt.title('img_histgram')
print(input_img.shape)

#ヒストグラムの正規化
number_pix = input_img.shape[0] * input_img.shape[1]
normalize_hist = np.zeros(img_hist.shape, dtype = float)
print(normalize_hist.shape)
for i in range(img_hist.shape[0]):
    normalize_hist[i][0] = img_hist[i][0] / number_pix

#グラフの塗りつぶし
hist_list = []
for i in range(normalize_hist.shape[0]):
    hist_list.append(normalize_hist[i][0])
plt.plot(np.arange(256), hist_list), plt.title('normalize_hist')
plt.fill_between(np.arange(256), hist_list, color = 'lightblue')

#ヒストグラム変換関数
def transform_function(normalize_hist):
    transform_hist = np.zeros(normalize_hist.shape, dtype = float)
    for i in range(normalize_hist.shape[0]):
        for j in range(i):
            transform_hist[i][0] += (normalize_hist.shape[0] - 1) * normalize_hist[j][0]

    #計算結果を少数地を丸める
    for i in range(normalize_hist.shape[0]):
        transform_hist[i][0] = round(transform_hist[i][0])

    return transform_hist

#変換関数を求め表示
t_function = transform_function(normalize_hist)
plt.plot(t_function), plt.title('transform_function')
print(t_function)

#新しいヒストグラムの作成
equalize_hist = np.zeros(t_function.shape, dtype = float)
for i in range(t_function.shape[0]):
    number = int(t_function[i][0])
    equalize_hist[number][0] = normalize_hist[i][0]

#グラフの塗りつぶし
my_list = []
for i in range(t_function.shape[0]):
    my_list.append(equalize_hist[i][0])

plt.subplot(111)
plt.plot(np.arange(256), my_list, linewidth = 0.5), plt.title('hist_equalization')
plt.fill_between(np.arange(256), my_list, color = 'lightblue')
