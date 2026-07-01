### 手写STL容器实现
- 整体目录结构主要包括alloc,container,test三个文件夹，build.sh用于构建可执行程序，check.sh用于检测内存泄漏
  - alloc目录下，放自定义的分配器，自定义定长内存池
  - container目录下，放自定义的vector\list\deque\stack\queue\rbtree\set\map的实现
  - test目录下，放对自定义分配器和自定义容器的简单测试


