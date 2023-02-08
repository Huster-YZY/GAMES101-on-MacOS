## 作业1总结
1.实现了model、projection变换。

2.实现了Rodrigues旋转公式。[参考链接](https://zhuanlan.zhihu.com/p/451579313)

## 框架错误：
rastrizer.cpp中的set\_pixel函数的ind计算公式为：
`auto ind = (height-point.y())*width + point.x();`

需要改为：
`auto ind = (height-point.y()-1)*width + point.x();`
因为point.y()的取值范围为[0,height),如果y为0，原来的公式计算出的ind大于height*width,会造成越界访问。