#### TODO:
## FRONTEND:
    - Implement DPI Awareness/Scaling for UI elements [ ]
    - Link loaded model data to UI Widgets [ ]
    - Stylize ImGUI (using some json files I assume) [ ]
    - Implement appropriate window scaling [ ]
        ~note: from .pconf file? -> window properties etc..
    - Implement docking using ImGUI [ ]

## BACKEND:
    - Fix camera movement [X]
    - Fix Scrolling [X]
    - Implement proper and simple keybind system [ ]
    
    # Feature Development:
    - Model data selection [ ]
    - Vertex/Edge/Face transformation [ ] <- save on copy of mdl
    - Revert model to original state [ ]
    # Server Development:
    - Create sever class which has its own UI to register users to database [ ]
    - Implement basic database functionality [ ]
    -- load and verify users from database [ ]
    -- create new users [ ]
    -- delete users [ ]
    -- assign privelages [ ]
    - Implement password hashing [ ]
    - Implement host/client privelage management [ ]
    - Data stream between clients and server allowing for RTC [ ]
    # Client Development:
    - Recieve and process server data/commands [ ]
    - ?
