import tkinter as tk

class WidgetCtl:
    def __init__(self):
        self._root = tk.Tk()
        self._root.title("NorSim")
        self._root.geometry("1200x600")
        self._root.resizable(False, False)
        self._root.protocol("WM_DELETE_WINDOW", lambda: self._close())
    
        # storage info page
        self._info_frame = tk.Frame(self._root)
        self._info_frame.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)
        self._info_frame.grid_propagate(True)
        self._info_frame.grid_rowconfigure(1, weight=1)

    def _close(self):
        self._root.destroy()

    def mainloop(self):
        self._root.mainloop()