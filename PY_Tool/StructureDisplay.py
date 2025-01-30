import tkinter as TK

# get base information from storage info section
class Storage_Display:
    def __init__(self, path):
        self.simfile_path = path
        print('File Path', path)
        self.init_state = True
        if not bool(self.simfile_path) and not 'sim' in self.simfile_path:
            self.init_state = False
        else :
            try:
                with open(self.simfile_path) as file:
                    self._sim_data = file.read()
            except Exception as Fill_Error:
                self.init_state = False
                print('File Open Error', Fill_Error)                

        # decode with base storage info
    
    def update_simdata(self):
        if self.init_state:
            # clear loaded file bytes first
            self._sim_data[:] = bytearray()

        

# create main widget