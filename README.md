# TextEditor
- 文本编辑器（支持使用命令对多个文本并发处理）

##有待完善
- 线程池的统一终止功能尚未实现
- replace功能尚未实现
- 保存功能并不稳定

##内部实现
- 使用线程池统一管理文本的并发处理
- 支持命令行对多个文本同时更改
- 使用mmapping直接获取文本内容，绕过文件流的限制
