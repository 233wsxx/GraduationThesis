import numpy as np
import matplotlib.pyplot as plt

# 生成 E/Q_bb 在区间 [0, 1.2] 上的网格点
x = np.linspace(0, 1.2, 500)

# ------------------------------------------------
# 1. 模拟 2νββ 能谱
#    这里用 x^2*(1 - x)^5 做示例，峰偏左，并做归一化
# ------------------------------------------------
f_2nu = x**2 * (1 - x)**5
f_2nu = f_2nu / np.max(f_2nu)  # 使最大值归一化为 1

# ------------------------------------------------
# 2. 模拟 0νββ 窄峰
#    使用高斯函数近似，中心在1，标准差较小
# ------------------------------------------------
sigma = 0.001  # 调节此值可改变峰的宽度
f_0nu = np.exp(-((x - 1)**2) / (2 * sigma**2))
f_0nu = f_0nu / 0.2  # 归一化到峰顶为 1
f_0nu = 0.2 * f_0nu  # 再缩放使其高度略低于 f_2nu 的峰值

# ------------------------------------------------
# 3. 绘图
# ------------------------------------------------
plt.figure(figsize=(10,6))
plt.plot(x, f_2nu, label=r'$2\nu\beta\beta$')
plt.plot(x, f_0nu, label=r'$0\nu\beta\beta$')

plt.xlabel(r'$E/Q_{\beta\beta}$', fontsize=14)
plt.ylabel(r'$dN/d(E/Q_{\beta\beta})$', fontsize=14)
plt.rcParams['font.sans-serif'] = ['SimHei']  # 指定中文字体
plt.rcParams['axes.unicode_minus'] = False     # 正常显示负号
plt.xlim(0, 1.2)
plt.ylim(0, 1.05)
plt.legend(fontsize=12)
# 去掉网格、x轴和y轴刻度数字
plt.grid(False)
plt.yticks([])
plt.show()
