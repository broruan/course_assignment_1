# 贪吃蛇游戏制作



# 1.Git 仓库使用方法

## 简介

Git 是一个分布式版本控制系统，用于跟踪文件的变化。本文档介绍如何使用 Git 仓库进行基本的版本控制操作。

## 基本概念

- **仓库 (Repository)**: 存储项目文件和历史记录的地方。
- **提交 (Commit)**: 保存文件当前状态的快照。
- **分支 (Branch)**: 独立的开发线。
- **远程仓库 (Remote)**: 托管在服务器上的仓库，如 GitHub。

## 基本命令

### 初始化仓库

```bash
git init
```

在项目目录中运行此命令来初始化一个新的 Git 仓库。

### 添加文件到暂存区

```bash
git add <文件名>
# 或添加所有文件
git add .
```

### 提交更改

```bash
git commit -m "提交信息"
```

### 查看状态

```bash
git status
```

### 查看提交历史

```bash
git log
```

### 连接远程仓库

```bash
git remote add origin <仓库URL>
```

### 推送更改

```bash
git push -u origin main
```

### 拉取更改

```bash
git pull
```

## 分支管理

### 创建分支

```bash
git branch <分支名>
```

### 切换分支

```bash
git checkout <分支名>
# 或使用新命令
git switch <分支名>
```

### 创建并切换到新分支

```bash
git checkout -b <分支名>
# 或
git switch -c <分支名>
```

### 合并分支

```bash
git merge <分支名>
```

## 常见工作流程

1. 创建或克隆仓库
2. 创建功能分支
3. 在分支上开发
4. 提交更改
5. 推送分支
6. 创建 Pull Request (如果使用 GitHub 等平台)
7. 合并到主分支

## 撤销更改

### 撤销工作目录的更改

```bash
git checkout -- <文件名>
```

### 撤销暂存区的更改

```bash
git reset HEAD <文件名>
```

### 撤销提交

```bash
git reset --soft HEAD~1  # 保留更改在暂存区
git reset --hard HEAD~1  # 完全删除更改
```

## 其他有用命令

- `git clone <URL>`: 克隆远程仓库
- `git diff`: 查看更改
- `git stash`: 临时保存更改
- `git fetch`: 获取远程更改但不合并

## 最佳实践

- 经常提交小更改
- 使用有意义的提交信息
- 在推送前拉取最新更改
- 使用分支进行功能开发
- 定期备份仓库

## 学习资源

- [Git 官方文档](https://git-scm.com/doc)
- [Pro Git 书籍](https://git-scm.com/book/zh/v2)
- [GitHub 学习路径](https://docs.github.com/cn/get-started/quickstart)

---


