#include "aspnmy_env.h"
#include <ctype.h>
#include <sys/stat.h>
#include <unistd.h>

#define COLOR_GREEN "\033[0;32m"
#define COLOR_YELLOW "\033[1;33m"
#define COLOR_BLUE "\033[0;34m"
#define COLOR_NC "\033[0m"

// 添加查找配置文件函数
char* find_env_file(void) {
    static char path[4096];
    char cwd[4096];
    char *current_dir = getcwd(cwd, sizeof(cwd));
    
    while (current_dir != NULL) {
        snprintf(path, sizeof(path), "%s/.env", current_dir);
        if (access(path, F_OK) == 0) {
            return path;
        }
        
        // 移动到父目录
        char *last_slash = strrchr(current_dir, '/');
        if (last_slash == current_dir) { // 到达根目录
            if (access("/.env", F_OK) == 0) {
                return "/.env";
            }
            break;
        } else if (last_slash != NULL) {
            *last_slash = '\0';
        } else {
            break;
        }
    }
    
    return NULL;
}

// 修改 load_env_file 函数
int load_env_file(const char* path, EnvVars* vars) {
    const char* env_path = path;
    if (strcmp(path, ".env") == 0) {
        env_path = find_env_file();
        if (env_path == NULL) {
            fprintf(stderr, "错误: 未找到 .env 文件\n");
            return -1;
        }
    }

    FILE* fp = fopen(env_path, "r");
    if (!fp) {
        fprintf(stderr, "错误: 无法打开配置文件 %s\n", env_path);
        return -1;
    }

    vars->count = 0;
    char line[MAX_LINE_LENGTH];

    while (fgets(line, sizeof(line), fp)) {
        // 跳过注释和空行
        if (line[0] == '#' || line[0] == '\n') continue;

        char* key = strtok(line, "=");
        char* value = strtok(NULL, "\n");
        
        if (key && value) {
            // 清理空格
            while (isspace(*key)) key++;
            while (isspace(*value)) value++;
            
            strncpy(vars->vars[vars->count].key, key, MAX_KEY_LENGTH - 1);
            strncpy(vars->vars[vars->count].value, value, MAX_VALUE_LENGTH - 1);
            
            // 导出到环境变量
            setenv(vars->vars[vars->count].key, vars->vars[vars->count].value, 1);
            
            vars->count++;
        }
    }

    fclose(fp);
    return 0;
}

// 生成 bashrc 配置
int save_to_bashrc(const EnvVars* vars) {
    char* home = getenv("HOME");
    if (!home) return -1;

    char bashrc_path[1024];
    snprintf(bashrc_path, sizeof(bashrc_path), "%s/.bashrc", home);

    // 创建临时文件
    char temp_path[1024];
    snprintf(temp_path, sizeof(temp_path), "%s/.bashrc.tmp", home);
    
    FILE* in = fopen(bashrc_path, "r");
    FILE* out = fopen(temp_path, "w");
    
    if (!in || !out) {
        if (in) fclose(in);
        if (out) fclose(out);
        return -1;
    }

    // 复制原文件内容，移除旧配置
    char line[MAX_LINE_LENGTH];
    int in_config = 0;
    
    while (fgets(line, sizeof(line), in)) {
        if (strstr(line, "BEGIN ASPNMY CONFIG")) {
            in_config = 1;
            continue;
        }
        if (strstr(line, "END ASPNMY CONFIG")) {
            in_config = 0;
            continue;
        }
        if (!in_config) {
            fputs(line, out);
        }
    }

    // 写入新配置
    fprintf(out, "\n# BEGIN ASPNMY CONFIG\ndeclare -A aspnmy\n\n");
    
    for (int i = 0; i < vars->count; i++) {
        fprintf(out, "aspnmy['%s']='%s'\n", 
               vars->vars[i].key, vars->vars[i].value);
    }
    
    fprintf(out, "\nexport aspnmy\n");
    fprintf(out, "get_config() { echo \"${aspnmy[$1]:-${!1}}\"; }\n");
    fprintf(out, "export -f get_config\n");
    fprintf(out, "# END ASPNMY CONFIG\n");

    fclose(in);
    fclose(out);

    // 替换原文件
    rename(temp_path, bashrc_path);
    return 0;
}

// 打印变量
void print_vars(const EnvVars* vars, int color) {
    if (color) {
        printf("%s========== ASPNMY 环境变量 ==========\n%s", COLOR_BLUE, COLOR_NC);
        printf("时间: ");
        fflush(stdout);
        system("date '+%Y-%m-%d %H:%M:%S'");
        printf("%s------------------------------------%s\n", COLOR_BLUE, COLOR_NC);
    } else {
        printf("========== ASPNMY 环境变量 ==========\n");
        printf("时间: ");
        fflush(stdout);
        system("date '+%Y-%m-%d %H:%M:%S'");
        printf("------------------------------------\n");
    }

    for (int i = 0; i < vars->count; i++) {
        if (color) {
            printf("%s%s%s=%s%s%s\n", 
                   COLOR_GREEN, vars->vars[i].key, COLOR_NC,
                   COLOR_YELLOW, vars->vars[i].value, COLOR_NC);
        } else {
            printf("%s=%s\n", vars->vars[i].key, vars->vars[i].value);
        }
    }

    if (color) {
        printf("%s------------------------------------%s\n", COLOR_BLUE, COLOR_NC);
        printf("共计: %s%d%s 个变量\n", COLOR_GREEN, vars->count, COLOR_NC);
    } else {
        printf("------------------------------------\n");
        printf("共计: %d 个变量\n", vars->count);
    }
}

// 修改 main 函数的调试输出
int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("用法: %s {install|uninstall|reload|print} [--no-color]\n", argv[0]);
        return 1;
    }

    EnvVars vars;
    int use_color = 1;
    
    if (argc > 2 && strcmp(argv[2], "--no-color") == 0) {
        use_color = 0;
    }

    if (strcmp(argv[1], "print") == 0) {
        char* env_path = find_env_file();
        if (env_path == NULL) {
            fprintf(stderr, "错误: 未找到 .env 文件\n");
            return 1;
        }
        printf("使用配置文件: %s\n", env_path);
        if (load_env_file(env_path, &vars) == 0) {
            print_vars(&vars, use_color);
        }
    } else if (strcmp(argv[1], "install") == 0) {
        if (load_env_file(".env", &vars) == 0) {
            save_to_bashrc(&vars);
            printf("环境变量已加载\n");
        }
    } else if (strcmp(argv[1], "reload") == 0) {
        system("sed -i '/^# BEGIN ASPNMY CONFIG/,/^# END ASPNMY CONFIG/d' ~/.bashrc");
        if (load_env_file(".env", &vars) == 0) {
            save_to_bashrc(&vars);
            printf("环境变量已重新加载\n");
        }
    } else if (strcmp(argv[1], "uninstall") == 0) {
        system("sed -i '/^# BEGIN ASPNMY CONFIG/,/^# END ASPNMY CONFIG/d' ~/.bashrc");
        printf("环境变量配置已移除\n");
    } else {
        printf("未知命令: %s\n", argv[1]);
        return 1;
    }

    return 0;
}
