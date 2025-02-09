import tkinter as tk
from tkinter import ttk

# # 创建主窗口
# root = tk.Tk()
# root.title("Treeview 显示滚动条示例")

# # 创建 Treeview 组件
# columns = ('列1', '列2', '列3', '列4', '列5')
# tree = ttk.Treeview(root, columns=columns, show='headings')

# # 设置列标题
# for col in columns:
#     tree.heading(col, text=col)
#     tree.column(col, width=100)

# # 插入大量示例数据
# for i in range(50):
#     values = [f"数据{i}-{j}" for j in range(len(columns))]
#     tree.insert('', 'end', values=values)

# # 创建垂直滚动条
# v_scrollbar = ttk.Scrollbar(root, orient=tk.VERTICAL, command=tree.yview)
# tree.configure(yscrollcommand=v_scrollbar.set)

# # 创建水平滚动条
# h_scrollbar = ttk.Scrollbar(root, orient=tk.HORIZONTAL, command=tree.xview)
# tree.configure(xscrollcommand=h_scrollbar.set)

# # 使用 grid 布局管理器进行布局
# tree.grid(row=0, column=0, sticky="nsew")
# v_scrollbar.grid(row=0, column=1, sticky="ns")
# h_scrollbar.grid(row=1, column=0, sticky="ew")

# # 设置行和列的权重，使 Treeview 可以随窗口大小调整
# root.rowconfigure(0, weight=1)
# root.columnconfigure(0, weight=1)

# # 运行主循环
# root.mainloop()


# 创建主窗口
root = tk.Tk()
root.title("主窗口")

# 创建 Frame 作为子窗口
frame = tk.Frame(root, borderwidth=2, relief="groove")
frame.pack(pady=20, padx=20)

# 在 Frame 子窗口中添加组件
label = tk.Label(frame, text="这是一个 Frame 子窗口")
label.pack(pady=20)

# 运行主循环
root.mainloop()