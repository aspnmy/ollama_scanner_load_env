# ollama_scanner_load_env

主要用于对项目全局变量env注入到系统环境变量的一个管理组件，.env文件位于项目根目录


## 使用说明

- 安装env文件中的全局变量到系统变量

  ```bash
  ./aspnmy_env install
  ```
- 卸载系统环境变量中全局变量

  ```bash
  ./aspnmy_env uninstall
  ```
- 重载系统环境中的变量

  ```bash
  ./aspnmy_env reload
  ```
- 打印系统环境中的项目全局变量

  ```bash
  ./aspnmy_env print
  ```

## .env文件的位置

- 必须位于项目根目录中
