import tkinter as tk
from tkinter import ttk

class EditableTreeview(ttk.Treeview):
    def __init__(self, master=None, **kw):
        super().__init__(master, **kw)
        self.bind("<Button-1>", self.on_click)
        self.entry = None
        self.item_id = None
        self.column_index = None

    def on_click(self, event):
        region = self.identify_region(event.x, event.y)
        if region == "cell":
            self.item_id = self.identify_row(event.y)
            self.column_index = self.identify_column(event.x)
            column = self.column_index[1:]
            value = self.set(self.item_id, column)
            x, y, width, height = self.bbox(self.item_id, column)
            if self.entry:
                self.entry.destroy()
            self.entry = tk.Entry(self, bd=0)
            self.entry.insert(0, value)
            self.entry.place(x=x, y=y, width=width, height=height)
            self.entry.focus()
            self.entry.bind("<Return>", self.on_enter)
            self.entry.bind("<Escape>", self.on_escape)

    def on_enter(self, event):
        new_value = self.entry.get()
        column = self.column_index[1:]
        self.set(self.item_id, column, new_value)
        self.entry.destroy()
        self.entry = None

    def on_escape(self, event):
        self.entry.destroy()
        self.entry = None


root = tk.Tk()

tree = EditableTreeview(root, columns=('Name', 'Age'), show='headings')
tree.heading('Name', text='Name')
tree.heading('Age', text='Age')
tree.pack()

tree.insert('', 'end', values=('Alice', 25))
tree.insert('', 'end', values=('Bob', 30))

root.mainloop()
