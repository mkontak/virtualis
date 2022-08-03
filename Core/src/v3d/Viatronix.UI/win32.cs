// $Id: win32.cs,v 1.14 2006/03/09 21:35:01 gdavidson Exp $
//
// Copyright(c) 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos (mailto:george@viatronix.com)

using System;
using System.Text;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Runtime.InteropServices;

namespace Win32
{
  #region enums


  public enum HookTypes
  {
    WH_MOUSE = 7,
    WH_MOUSE_LL = 14,
  }

  public enum SysCommands
  {
    SC_SIZE         = 0xF000,
    SC_MOVE         = 0xF010,
    SC_MINIMIZE     = 0xF020,
    SC_MAXIMIZE     = 0xF030,
    SC_NEXTWINDOW   = 0xF040,
    SC_PREVWINDOW   = 0xF050,
    SC_CLOSE        = 0xF060,
    SC_VSCROLL      = 0xF070,
    SC_HSCROLL      = 0xF080,
    SC_MOUSEMENU    = 0xF090,
    SC_KEYMENU      = 0xF100,
    SC_ARRANGE      = 0xF110,
    SC_RESTORE      = 0xF120,
    SC_TASKLIST     = 0xF130,
    SC_SCREENSAVE   = 0xF140,
    SC_HOTKEY       = 0xF150,
    SC_DEFAULT      = 0xF160,
    SC_MONITORPOWER = 0xF170,
    SC_CONTEXTHELP  = 0xF180,
    SC_SEPARATOR    = 0xF00F,
  }
  
  // custom draw return flags
  public enum CDRF : uint
  {
    CDRF_DODEFAULT          = 0x00000000,
    CDRF_NEWFONT            = 0x00000002,
    CDRF_SKIPDEFAULT        = 0x00000004,
    CDRF_NOTIFYPOSTPAINT    = 0x00000010,
    CDRF_NOTIFYITEMDRAW     = 0x00000020,
    CDRF_NOTIFYPOSTERASE    = 0x00000040,
  }

  public enum CDDS : uint
  {
    CDDS_PREPAINT           = 0x00000001,
    CDDS_POSTPAINT          = 0x00000002,
    CDDS_PREERASE           = 0x00000003,
    CDDS_POSTERASE          = 0x00000004,
    CDDS_ITEM               = 0x00010000,
    CDDS_ITEMPREPAINT       = (CDDS_ITEM | CDDS_PREPAINT),
    CDDS_ITEMPOSTPAINT      = (CDDS_ITEM | CDDS_POSTPAINT),
    CDDS_ITEMPREERASE       = (CDDS_ITEM | CDDS_PREERASE),
    CDDS_ITEMPOSTERASE      = (CDDS_ITEM | CDDS_POSTERASE),
    CDDS_SUBITEM            = 0x00020000,
  }

  public enum TPM 
  {
    TPM_LEFTBUTTON      = 0x0000,
    TPM_RIGHTBUTTON     = 0x0002,
    TPM_LEFTALIGN       = 0x0000,
    TPM_CENTERALIGN     = 0x0004,
    TPM_RIGHTALIGN      = 0x0008,
    TPM_TOPALIGN        = 0x0000,
    TPM_VCENTERALIGN    = 0x0010,
    TPM_BOTTOMALIGN     = 0x0020,
    TPM_HORIZONTAL      = 0x0000,
    TPM_VERTICAL        = 0x0040,
    TPM_NONOTIFY        = 0x0080,
    TPM_RETURNCMD       = 0x0100,
    TPM_RECURSE         = 0x0001,
    TPM_HORPOSANIMATION = 0x0400,
    TPM_HORNEGANIMATION = 0x0800,
    TPM_VERPOSANIMATION = 0x1000,
    TPM_VERNEGANIMATION = 0x2000,
    TPM_NOANIMATION     = 0x4000,
    TPM_LAYOUTRTL       = 0x8000,
  }


  [FlagsAttribute]
  public enum SHGFI : uint
  {
    SHGFI_ICON              = 0x000000100, 
    SHGFI_DISPLAYNAME       = 0x000000200, 
    SHGFI_TYPENAME          = 0x000000400, 
    SHGFI_ATTRIBUTES        = 0x000000800, 
    SHGFI_ICONLOCATION      = 0x000001000, 
    SHGFI_EXETYPE           = 0x000002000, 
    SHGFI_SYSICONINDEX      = 0x000004000, 
    SHGFI_LINKOVERLAY       = 0x000008000, 
    SHGFI_SELECTED          = 0x000010000, 
    SHGFI_ATTR_SPECIFIED    = 0x000020000,
    SHGFI_LARGEICON         = 0x000000000,
    SHGFI_SMALLICON         = 0x000000001, 
    SHGFI_OPENICON          = 0x000000002,
    SHGFI_SHELLICONSIZE     = 0x000000004, 
    SHGFI_PIDL              = 0x000000008, 
    SHGFI_USEFILEATTRIBUTES = 0x000000010,
    SHGFI_ADDOVERLAYS       = 0x000000020, 
    SHGFI_OVERLAYINDEX      = 0x000000040, 
  }


  [FlagsAttribute]
  public enum ILD : uint
  {
    
    ILD_NORMAL              = 0x00000000,
    ILD_TRANSPARENT         = 0x00000001,
    ILD_MASK                = 0x00000010,
    ILD_IMAGE               = 0x00000020,
    ILD_ROP                 = 0x00000040,
    ILD_BLEND25             = 0x00000002,
    ILD_BLEND50             = 0x00000004,
    ILD_OVERLAYMASK         = 0x00000F00,
    ILD_PRESERVEALPHA       = 0x00001000,
    ILD_SCALE               = 0x00002000, 
    ILD_DPISCALE            = 0x00004000,
    ILD_SELECTED            = ILD_BLEND50,
    ILD_FOCUS               = ILD_BLEND25,
    ILD_BLEND               = ILD_BLEND50,
    
  }


  [FlagsAttribute]
  public enum FILE : uint
  {
    FILE_ATTRIBUTE_READONLY  = 0x00000001,
    FILE_ATTRIBUTE_HIDDEN    = 0x00000002,
    FILE_ATTRIBUTE_SYSTEM    = 0x00000004,
    FILE_ATTRIBUTE_DIRECTORY = 0x00000010,
    FILE_ATTRIBUTE_ARCHIVE   = 0x00000020,
    FILE_ATTRIBUTE_NORMAL    = 0x00000080,
    FILE_ATTRIBUTE_TEMPORARY = 0x00000100,
  }


  [FlagsAttribute]
  public enum LVM
  {
    LVM_FIRST                     = 0x1000,      // ListView messages
    LVM_GETBKCOLOR                = (LVM_FIRST + 0),
    LVM_SETBKCOLOR                = (LVM_FIRST + 1),
    LVM_GETIMAGELIST              = (LVM_FIRST + 2),
    LVM_SETIMAGELIST              = (LVM_FIRST + 3),
    LVM_GETITEMCOUNT              = (LVM_FIRST + 4),
    LVM_GETITEMA                  = (LVM_FIRST + 5),
    LVM_GETITEMW                  = (LVM_FIRST + 75),
    LVM_SETITEMA                  = (LVM_FIRST + 6),
    LVM_SETITEMW                  = (LVM_FIRST + 76),
    LVM_INSERTITEMA               = (LVM_FIRST + 7),
    LVM_INSERTITEMW               = (LVM_FIRST + 77),
    LVM_INSERTITEM                = LVM_INSERTITEMW,
    LVM_DELETEITEM                = (LVM_FIRST + 8),
    LVM_DELETEALLITEMS            = (LVM_FIRST + 9),
    LVM_GETCALLBACKMASK           = (LVM_FIRST + 10),
    LVM_SETCALLBACKMASK           = (LVM_FIRST + 11),
    LVM_GETNEXTITEM               = (LVM_FIRST + 12),
    LVM_FINDITEMA                 = (LVM_FIRST + 13),
    LVM_FINDITEMW                 = (LVM_FIRST + 83),
    LVM_GETITEMRECT               = (LVM_FIRST + 14),
    LVM_SETITEMPOSITION           = (LVM_FIRST + 15),
    LVM_GETITEMPOSITION           = (LVM_FIRST + 16),
    LVM_GETSTRINGWIDTHA           = (LVM_FIRST + 17),
    LVM_GETSTRINGWIDTHW           = (LVM_FIRST + 87),
    LVM_HITTEST                   = (LVM_FIRST + 18),
    LVM_ENSUREVISIBLE             = (LVM_FIRST + 19),
    LVM_SCROLL                    = (LVM_FIRST + 20),
    LVM_REDRAWITEMS               = (LVM_FIRST + 21),
    LVM_ARRANGE                   = (LVM_FIRST + 22),
    LVM_EDITLABELA                = (LVM_FIRST + 23),
    LVM_EDITLABELW                = (LVM_FIRST + 118),
    LVM_GETEDITCONTROL            = (LVM_FIRST + 24),
    LVM_GETCOLUMNA                = (LVM_FIRST + 25),
    LVM_GETCOLUMNW                = (LVM_FIRST + 95),
    LVM_SETCOLUMNA                = (LVM_FIRST + 26),
    LVM_SETCOLUMNW                = (LVM_FIRST + 96),
    LVM_INSERTCOLUMNA             = (LVM_FIRST + 27),
    LVM_INSERTCOLUMNW             = (LVM_FIRST + 97),
    LVM_DELETECOLUMN              = (LVM_FIRST + 28),
    LVM_GETCOLUMNWIDTH            = (LVM_FIRST + 29),
    LVM_SETCOLUMNWIDTH            = (LVM_FIRST + 30),
    LVM_GETHEADER                 = (LVM_FIRST + 31),
    LVM_CREATEDRAGIMAGE           = (LVM_FIRST + 33),
    LVM_GETVIEWRECT               = (LVM_FIRST + 34),
    LVM_GETTEXTCOLOR              = (LVM_FIRST + 35),
    LVM_SETTEXTCOLOR              = (LVM_FIRST + 36),
    LVM_GETTEXTBKCOLOR            = (LVM_FIRST + 37),
    LVM_SETTEXTBKCOLOR            = (LVM_FIRST + 38),
    LVM_GETTOPINDEX               = (LVM_FIRST + 39),
    LVM_GETCOUNTPERPAGE           = (LVM_FIRST + 40),
    LVM_GETORIGIN                 = (LVM_FIRST + 41),
    LVM_UPDATE                    = (LVM_FIRST + 42),
    LVM_SETITEMSTATE              = (LVM_FIRST + 43),
    LVM_GETITEMSTATE              = (LVM_FIRST + 44),
    LVM_GETITEMTEXTA              = (LVM_FIRST + 45),
    LVM_GETITEMTEXTW              = (LVM_FIRST + 115),
    LVM_SETITEMTEXTA              = (LVM_FIRST + 46),
    LVM_SETITEMTEXTW              = (LVM_FIRST + 116),
    LVM_SETITEMCOUNT              = (LVM_FIRST + 47),
    LVM_SORTITEMS                 = (LVM_FIRST + 48),
    LVM_SETITEMPOSITION32         = (LVM_FIRST + 49),
    LVM_GETSELECTEDCOUNT          = (LVM_FIRST + 50),
    LVM_GETITEMSPACING            = (LVM_FIRST + 51),
    LVM_GETISEARCHSTRINGA         = (LVM_FIRST + 52),
    LVM_GETISEARCHSTRINGW         = (LVM_FIRST + 117),
    LVM_SETICONSPACING            = (LVM_FIRST + 53),
    LVM_SETEXTENDEDLISTVIEWSTYLE  = (LVM_FIRST + 54), 
    LVM_GETEXTENDEDLISTVIEWSTYLE  = (LVM_FIRST + 55),
    LVM_GETSUBITEMRECT            = (LVM_FIRST + 56),
    LVM_SUBITEMHITTEST            = (LVM_FIRST + 57),
    LVM_SETCOLUMNORDERARRAY       = (LVM_FIRST + 58),
    LVM_GETCOLUMNORDERARRAY       = (LVM_FIRST + 59),
    LVM_SETHOTITEM                = (LVM_FIRST + 60),
    LVM_GETHOTITEM                = (LVM_FIRST + 61),
    LVM_SETHOTCURSOR              = (LVM_FIRST + 62),
    LVM_GETHOTCURSOR              = (LVM_FIRST + 63),
    LVM_APPROXIMATEVIEWRECT       = (LVM_FIRST + 64),
    LVM_SETWORKAREAS              = (LVM_FIRST + 65),
    LVM_GETWORKAREAS              = (LVM_FIRST + 70),
    LVM_GETNUMBEROFWORKAREAS      = (LVM_FIRST + 73),
    LVM_GETSELECTIONMARK          = (LVM_FIRST + 66),
    LVM_SETSELECTIONMARK          = (LVM_FIRST + 67),
    LVM_SETHOVERTIME              = (LVM_FIRST + 71),
    LVM_GETHOVERTIME              = (LVM_FIRST + 72),
    LVM_SETTOOLTIPS               = (LVM_FIRST + 74),
    LVM_GETTOOLTIPS               = (LVM_FIRST + 78),
    LVM_SORTITEMSEX               = (LVM_FIRST + 81),
    LVM_SETBKIMAGEA               = (LVM_FIRST + 68),
    LVM_SETBKIMAGEW               = (LVM_FIRST + 138),
    LVM_GETBKIMAGEA               = (LVM_FIRST + 69),
    LVM_GETBKIMAGEW               = (LVM_FIRST + 139),
    LVM_SETSELECTEDCOLUMN         = (LVM_FIRST + 140),
    LVM_SETTILEWIDTH              = (LVM_FIRST + 141),
    LVM_SETVIEW                   = (LVM_FIRST + 142),
    LVM_GETVIEW                   = (LVM_FIRST + 143),
    LVM_INSERTGROUP               = (LVM_FIRST + 145),
    LVM_SETGROUPINFO              = (LVM_FIRST + 147),
    LVM_GETGROUPINFO              = (LVM_FIRST + 149),
    LVM_REMOVEGROUP               = (LVM_FIRST + 150),
    LVM_MOVEGROUP                 = (LVM_FIRST + 151),
    LVM_MOVEITEMTOGROUP           = (LVM_FIRST + 154),
    LVM_SETGROUPMETRICS           = (LVM_FIRST + 155),
    LVM_GETGROUPMETRICS           = (LVM_FIRST + 156),
    LVM_ENABLEGROUPVIEW           = (LVM_FIRST + 157),
    LVM_SORTGROUPS                = (LVM_FIRST + 158),
    LVM_INSERTGROUPSORTED         = (LVM_FIRST + 159),
    LVM_REMOVEALLGROUPS           = (LVM_FIRST + 160),
    LVM_HASGROUP                  = (LVM_FIRST + 161),
    LVM_SETTILEVIEWINFO           = (LVM_FIRST + 162),
    LVM_GETTILEVIEWINFO           = (LVM_FIRST + 163),
    LVM_SETTILEINFO               = (LVM_FIRST + 164),
    LVM_GETTILEINFO               = (LVM_FIRST + 165),
    LVM_SETINSERTMARK             = (LVM_FIRST + 166),
    LVM_GETINSERTMARK             = (LVM_FIRST + 167),
    LVM_INSERTMARKHITTEST         = (LVM_FIRST + 168),
    LVM_GETINSERTMARKRECT         = (LVM_FIRST + 169),
    LVM_SETINSERTMARKCOLOR        = (LVM_FIRST + 170),
    LVM_GETINSERTMARKCOLOR        = (LVM_FIRST + 171),
    LVM_SETINFOTIP                = (LVM_FIRST + 173),
    LVM_GETSELECTEDCOLUMN         = (LVM_FIRST + 174),
    LVM_ISGROUPVIEWENABLED        = (LVM_FIRST + 175),
    LVM_GETOUTLINECOLOR           = (LVM_FIRST + 176),
    LVM_SETOUTLINECOLOR           = (LVM_FIRST + 177),
    LVM_CANCELEDITLABEL           = (LVM_FIRST + 179),
    LVM_MAPINDEXTOID              = (LVM_FIRST + 180),
    LVM_MAPIDTOINDEX              = (LVM_FIRST + 181),
  }


  [FlagsAttribute]
  public enum LVS_EX
  {
    LVS_EX_GRIDLINES        =0x00000001,
    LVS_EX_SUBITEMIMAGES    =0x00000002,
    LVS_EX_CHECKBOXES       =0x00000004,
    LVS_EX_TRACKSELECT      =0x00000008,
    LVS_EX_HEADERDRAGDROP   =0x00000010,
    LVS_EX_FULLROWSELECT    =0x00000020, 
    LVS_EX_ONECLICKACTIVATE =0x00000040,
    LVS_EX_TWOCLICKACTIVATE =0x00000080,
    LVS_EX_FLATSB           =0x00000100,
    LVS_EX_REGIONAL         =0x00000200,
    LVS_EX_INFOTIP          =0x00000400,
    LVS_EX_UNDERLINEHOT     =0x00000800,
    LVS_EX_UNDERLINECOLD    =0x00001000,
    LVS_EX_MULTIWORKAREAS   =0x00002000,
    LVS_EX_LABELTIP         =0x00004000,
    LVS_EX_BORDERSELECT     =0x00008000, 
    LVS_EX_DOUBLEBUFFER     =0x00010000,
    LVS_EX_HIDELABELS       =0x00020000,
    LVS_EX_SINGLEROW        =0x00040000,
    LVS_EX_SNAPTOGRID       =0x00080000,  
    LVS_EX_SIMPLESELECT     =0x00100000  
  }


  [FlagsAttribute]
  public enum LVS
  {
    LVS_ICON               = 0x0000,
    LVS_REPORT             = 0x0001,
    LVS_SMALLICON          = 0x0002,
    LVS_LIST               = 0x0003,
    LVS_TYPEMASK           = 0x0003,
    LVS_SINGLESEL          = 0x0004,
    LVS_SHOWSELALWAYS      = 0x0008,
    LVS_SORTASCENDING      = 0x0010,
    LVS_SORTDESCENDING     = 0x0020,
    LVS_SHAREIMAGELISTS    = 0x0040,
    LVS_NOLABELWRAP        = 0x0080,
    LVS_AUTOARRANGE        = 0x0100,
    LVS_EDITLABELS         = 0x0200,
    LVS_OWNERDATA          = 0x1000,
    LVS_NOSCROLL           = 0x2000,
    LVS_TYPESTYLEMASK      = 0xfc00,
    LVS_ALIGNTOP           = 0x0000,
    LVS_ALIGNLEFT          = 0x0800,
    LVS_ALIGNMASK          = 0x0c00,
    LVS_OWNERDRAWFIXED     = 0x0400,
    LVS_NOCOLUMNHEADER     = 0x4000,
    LVS_NOSORTHEADER       = 0x8000,
  }


  [FlagsAttribute]
  public enum LVGF
  {
    LVGF_NONE         = 0x00000000,
    LVGF_HEADER       = 0x00000001,
    LVGF_FOOTER       = 0x00000002,
    LVGF_STATE        = 0x00000004,
    LVGF_ALIGN        = 0x00000008,
    LVGF_GROUPID      = 0x00000010,
  }


  [FlagsAttribute]
  public enum LVGS
  {
    LVGS_NORMAL         = 0x00000000,
    LVGS_COLLAPSED      = 0x00000001,
    LVGS_HIDDEN         = 0x00000002,
  }


  [FlagsAttribute]
  public enum LVGA
  {
    LVGA_HEADER_LEFT    = 0x00000001,
    LVGA_HEADER_CENTER  = 0x00000002,
    LVGA_HEADER_RIGHT   = 0x00000004,  // Don't forget to validate exclusivity
    LVGA_FOOTER_LEFT    = 0x00000008,
    LVGA_FOOTER_CENTER  = 0x00000010,
    LVGA_FOOTER_RIGHT   = 0x00000020,  // Don't forget to validate exclusivity
  }


  [FlagsAttribute]
  public enum LVIS
  {
    LVIS_FOCUSED          =  0x0001,
    LVIS_SELECTED         =  0x0002,
    LVIS_CUT              =  0x0004,
    LVIS_DROPHILITED      =  0x0008,
    LVIS_GLOW             =  0x0010,
    LVIS_ACTIVATING       =  0x0020,
    LVIS_OVERLAYMASK      =  0x0F00,
    LVIS_STATEIMAGEMASK   =  0xF000,
  }


  [FlagsAttribute]
  public enum LVIF
  {
    LVIF_TEXT               = 0x0001,
    LVIF_IMAGE              = 0x0002,
    LVIF_PARAM              = 0x0004,
    LVIF_STATE              = 0x0008,
    LVIF_INDENT             = 0x0010,
    LVIF_NORECOMPUTE        = 0x0800,
    LVIF_GROUPID            = 0x0100,
    LVIF_COLUMNS            = 0x0200,
  }

  public enum LVCDI
  {
    LVCDI_ITEM      = 0x00000000,
    LVCDI_GROUP     = 0x00000001,
  } // LVCDI


  [FlagsAttribute]
  public enum LVCF
  {
    LVCF_FMT      = 0x0001,
    LVCF_WIDTH    = 0x0002,
    LVCF_TEXT     = 0x0004,
    LVCF_SUBITEM  = 0x0008,
    LVCF_IMAGE    = 0x0010,
    LVCF_ORDER    = 0x0020,
  }


  public enum LVCFMT
  {
    LVCFMT_LEFT             = 0x0000,
    LVCFMT_RIGHT            = 0x0001,
    LVCFMT_CENTER           = 0x0002,
    LVCFMT_JUSTIFYMASK      = 0x0003,
    LVCFMT_IMAGE            = 0x0800,
    LVCFMT_BITMAP_ON_RIGHT  = 0x1000,
    LVCFMT_COL_HAS_IMAGES   = 0x8000,
  }

  #region PeekMessageFlags
  
  public enum PeekMessageFlags
  {
    PM_NOREMOVE		= 0,
    PM_REMOVE		= 1,
    PM_NOYIELD		= 2
  } // enum PeekMessageFlags

  #endregion

  #region KeyStateFlags
  
  public enum KeyStateFlags : uint
  {
    MK_LBUTTON          = 0x0001,
    MK_RBUTTON          = 0x0002,
    MK_SHIFT            = 0x0004,
    MK_CONTROL          = 0x0008,
    MK_MBUTTON          = 0x0010,
    MK_XBUTTON1         = 0x0020,
    MK_XBUTTON2         = 0x0040,
  } // enum KeyStateFlags 

  #endregion
   
  #region SetWindowPosFlags
  
  public enum SetWindowPosFlags : uint
  {
    SWP_NOSIZE          = 0x0001,
    SWP_NOMOVE          = 0x0002,
    SWP_NOZORDER        = 0x0004,
    SWP_NOREDRAW        = 0x0008,
    SWP_NOACTIVATE      = 0x0010,
    SWP_FRAMECHANGED    = 0x0020,
    SWP_SHOWWINDOW      = 0x0040,
    SWP_HIDEWINDOW      = 0x0080,
    SWP_NOCOPYBITS      = 0x0100,
    SWP_NOOWNERZORDER   = 0x0200, 
    SWP_NOSENDCHANGING  = 0x0400,
    SWP_DRAWFRAME       = 0x0020,
    SWP_NOREPOSITION    = 0x0200,
    SWP_DEFERERASE      = 0x2000,
    SWP_ASYNCWINDOWPOS  = 0x4000
  } // enum SetWindowPosFlags

  #endregion

  #region SetWindowPosZ
  
  public enum SetWindowPosZ 
  {
    HWND_TOP        = 0,
    HWND_BOTTOM     = 1,
    HWND_TOPMOST    = -1,
    HWND_NOTOPMOST  = -2
  } // enum SetWindowPosZ 

  #endregion

  #region ShowWindowStyles
  
  public enum ShowWindowStyles : short
  {
    SW_HIDE             = 0,
    SW_SHOWNORMAL       = 1,
    SW_NORMAL           = 1,
    SW_SHOWMINIMIZED    = 2,
    SW_SHOWMAXIMIZED    = 3,
    SW_MAXIMIZE         = 3,
    SW_SHOWNOACTIVATE   = 4,
    SW_SHOW             = 5,
    SW_MINIMIZE         = 6,
    SW_SHOWMINNOACTIVE  = 7,
    SW_SHOWNA           = 8,
    SW_RESTORE          = 9,
    SW_SHOWDEFAULT      = 10,
    SW_FORCEMINIMIZE    = 11,
    SW_MAX              = 11
  } // enum ShowWindowStyles

  #endregion

  #region WindowStyles
  
  public enum WindowStyles : uint
  {
    WS_OVERLAPPED       = 0x00000000,
    WS_POPUP            = 0x80000000,
    WS_CHILD            = 0x40000000,
    WS_MINIMIZE         = 0x20000000,
    WS_VISIBLE          = 0x10000000,
    WS_DISABLED         = 0x08000000,
    WS_CLIPSIBLINGS     = 0x04000000,
    WS_CLIPCHILDREN     = 0x02000000,
    WS_MAXIMIZE         = 0x01000000,
    WS_CAPTION          = 0x00C00000,
    WS_BORDER           = 0x00800000,
    WS_DLGFRAME         = 0x00400000,
    WS_VSCROLL          = 0x00200000,
    WS_HSCROLL          = 0x00100000,
    WS_SYSMENU          = 0x00080000,
    WS_THICKFRAME       = 0x00040000,
    WS_GROUP            = 0x00020000,
    WS_TABSTOP          = 0x00010000,
    WS_MINIMIZEBOX      = 0x00020000,
    WS_MAXIMIZEBOX      = 0x00010000,
    WS_TILED            = 0x00000000,
    WS_ICONIC           = 0x20000000,
    WS_SIZEBOX          = 0x00040000,
    WS_POPUPWINDOW      = 0x80880000,
    WS_OVERLAPPEDWINDOW = 0x00CF0000,
    WS_TILEDWINDOW      = 0x00CF0000,
    WS_CHILDWINDOW      = 0x40000000,
    CCS_TOP             = 0x00000001U,
    CCS_NOMOVEY         = 0x00000002U,
    CCS_BOTTOM          = 0x00000003U,
    CCS_NORESIZE        = 0x00000004U,
    CCS_NOPARENTALIGN   = 0x00000008U,
    CCS_ADJUSTABLE      = 0x00000020U,
    CCS_NODIVIDER       = 0x00000040U,
    CCS_VERT            = 0x00000080U,
    CCS_LEFT            = (CCS_VERT | CCS_TOP),
    CCS_RIGHT           = (CCS_VERT | CCS_BOTTOM),
    CCS_NOMOVEX         = (CCS_VERT | CCS_NOMOVEY),
    RBS_TOOLTIPS        = 0x0100,
    RBS_VARHEIGHT       = 0x0200,
    RBS_BANDBORDERS     = 0x0400,
    RBS_FIXEDORDER      = 0x0800,
    RBS_REGISTERDROP    = 0x1000,
    RBS_AUTOSIZE        = 0x2000,
    RBS_VERTICALGRIPPER = 0x4000,
    RBS_DBLCLKTOGGLE    = 0x8000,
  } // enum WindowStyles 

  #endregion

  #region WindowExStyles

  public enum WindowExStyles
  {
    WS_EX_DLGMODALFRAME     = 0x00000001,
    WS_EX_NOPARENTNOTIFY    = 0x00000004,
    WS_EX_TOPMOST           = 0x00000008,
    WS_EX_ACCEPTFILES       = 0x00000010,
    WS_EX_TRANSPARENT       = 0x00000020,
    WS_EX_MDICHILD          = 0x00000040,
    WS_EX_TOOLWINDOW        = 0x00000080,
    WS_EX_WINDOWEDGE        = 0x00000100,
    WS_EX_CLIENTEDGE        = 0x00000200,
    WS_EX_CONTEXTHELP       = 0x00000400,
    WS_EX_RIGHT             = 0x00001000,
    WS_EX_LEFT              = 0x00000000,
    WS_EX_RTLREADING        = 0x00002000,
    WS_EX_LTRREADING        = 0x00000000,
    WS_EX_LEFTSCROLLBAR     = 0x00004000,
    WS_EX_RIGHTSCROLLBAR    = 0x00000000,
    WS_EX_CONTROLPARENT     = 0x00010000,
    WS_EX_STATICEDGE        = 0x00020000,
    WS_EX_APPWINDOW         = 0x00040000,
    WS_EX_OVERLAPPEDWINDOW  = 0x00000300,
    WS_EX_PALETTEWINDOW     = 0x00000188,
    WS_EX_LAYERED			      = 0x00080000
  } // enum WindowExStyles

  
  #endregion

  #region VirtualKeys

  public enum VirtualKeys
  {
    VK_LBUTTON		= 0x01,
    VK_CANCEL		= 0x03,
    VK_BACK			= 0x08,
    VK_TAB			= 0x09,
    VK_CLEAR		= 0x0C,
    VK_RETURN		= 0x0D,
    VK_SHIFT		= 0x10,
    VK_CONTROL		= 0x11,
    VK_MENU			= 0x12,
    VK_CAPITAL		= 0x14,
    VK_ESCAPE		= 0x1B,
    VK_SPACE		= 0x20,
    VK_PRIOR		= 0x21,
    VK_NEXT			= 0x22,
    VK_END			= 0x23,
    VK_HOME			= 0x24,
    VK_LEFT			= 0x25,
    VK_UP			= 0x26,
    VK_RIGHT		= 0x27,
    VK_DOWN			= 0x28,
    VK_SELECT		= 0x29,
    VK_EXECUTE		= 0x2B,
    VK_SNAPSHOT		= 0x2C,
    VK_HELP			= 0x2F,
    VK_0			= 0x30,
    VK_1			= 0x31,
    VK_2			= 0x32,
    VK_3			= 0x33,
    VK_4			= 0x34,
    VK_5			= 0x35,
    VK_6			= 0x36,
    VK_7			= 0x37,
    VK_8			= 0x38,
    VK_9			= 0x39,
    VK_A			= 0x41,
    VK_B			= 0x42,
    VK_C			= 0x43,
    VK_D			= 0x44,
    VK_E			= 0x45,
    VK_F			= 0x46,
    VK_G			= 0x47,
    VK_H			= 0x48,
    VK_I			= 0x49,
    VK_J			= 0x4A,
    VK_K			= 0x4B,
    VK_L			= 0x4C,
    VK_M			= 0x4D,
    VK_N			= 0x4E,
    VK_O			= 0x4F,
    VK_P			= 0x50,
    VK_Q			= 0x51,
    VK_R			= 0x52,
    VK_S			= 0x53,
    VK_T			= 0x54,
    VK_U			= 0x55,
    VK_V			= 0x56,
    VK_W			= 0x57,
    VK_X			= 0x58,
    VK_Y			= 0x59,
    VK_Z			= 0x5A,
    VK_NUMPAD0		= 0x60,
    VK_NUMPAD1		= 0x61,
    VK_NUMPAD2		= 0x62,
    VK_NUMPAD3		= 0x63,
    VK_NUMPAD4		= 0x64,
    VK_NUMPAD5		= 0x65,
    VK_NUMPAD6		= 0x66,
    VK_NUMPAD7		= 0x67,
    VK_NUMPAD8		= 0x68,
    VK_NUMPAD9		= 0x69,
    VK_MULTIPLY		= 0x6A,
    VK_ADD			= 0x6B,
    VK_SEPARATOR	= 0x6C,
    VK_SUBTRACT		= 0x6D,
    VK_DECIMAL		= 0x6E,
    VK_DIVIDE		= 0x6F,
    VK_ATTN			= 0xF6,
    VK_CRSEL		= 0xF7,
    VK_EXSEL		= 0xF8,
    VK_EREOF		= 0xF9,
    VK_PLAY			= 0xFA,  
    VK_ZOOM			= 0xFB,
    VK_NONAME		= 0xFC,
    VK_PA1			= 0xFD,
    VK_OEM_CLEAR	= 0xFE,
    VK_LWIN			= 0x5B,
    VK_RWIN			= 0x5C,
    VK_APPS			= 0x5D,   
    VK_LSHIFT		= 0xA0,   
    VK_RSHIFT		= 0xA1,   
    VK_LCONTROL		= 0xA2,   
    VK_RCONTROL		= 0xA3,   
    VK_LMENU		= 0xA4,   
    VK_RMENU		= 0xA5
  } // enum VirtualKeys

  #endregion

  #region Msgs

  public enum Msgs
  {
    WM_NULL                   = 0x0000,
    WM_CREATE                 = 0x0001,
    WM_DESTROY                = 0x0002,
    WM_MOVE                   = 0x0003,
    WM_SIZE                   = 0x0005,
    WM_ACTIVATE               = 0x0006,
    WM_SETFOCUS               = 0x0007,
    WM_KILLFOCUS              = 0x0008,
    WM_ENABLE                 = 0x000A,
    WM_SETREDRAW              = 0x000B,
    WM_SETTEXT                = 0x000C,
    WM_GETTEXT                = 0x000D,
    WM_GETTEXTLENGTH          = 0x000E,
    WM_PAINT                  = 0x000F,
    WM_CLOSE                  = 0x0010,
    WM_QUERYENDSESSION        = 0x0011,
    WM_QUIT                   = 0x0012,
    WM_QUERYOPEN              = 0x0013,
    WM_ERASEBKGND             = 0x0014,
    WM_SYSCOLORCHANGE         = 0x0015,
    WM_ENDSESSION             = 0x0016,
    WM_SHOWWINDOW             = 0x0018,
    WM_WININICHANGE           = 0x001A,
    WM_SETTINGCHANGE          = 0x001A,
    WM_DEVMODECHANGE          = 0x001B,
    WM_ACTIVATEAPP            = 0x001C,
    WM_FONTCHANGE             = 0x001D,
    WM_TIMECHANGE             = 0x001E,
    WM_CANCELMODE             = 0x001F,
    WM_SETCURSOR              = 0x0020,
    WM_MOUSEACTIVATE          = 0x0021,
    WM_CHILDACTIVATE          = 0x0022,
    WM_QUEUESYNC              = 0x0023,
    WM_GETMINMAXINFO          = 0x0024,
    WM_PAINTICON              = 0x0026,
    WM_ICONERASEBKGND         = 0x0027,
    WM_NEXTDLGCTL             = 0x0028,
    WM_SPOOLERSTATUS          = 0x002A,
    WM_DRAWITEM               = 0x002B,
    WM_MEASUREITEM            = 0x002C,
    WM_DELETEITEM             = 0x002D,
    WM_VKEYTOITEM             = 0x002E,
    WM_CHARTOITEM             = 0x002F,
    WM_SETFONT                = 0x0030,
    WM_GETFONT                = 0x0031,
    WM_SETHOTKEY              = 0x0032,
    WM_GETHOTKEY              = 0x0033,
    WM_QUERYDRAGICON          = 0x0037,
    WM_COMPAREITEM            = 0x0039,
    WM_GETOBJECT              = 0x003D,
    WM_COMPACTING             = 0x0041,
    WM_COMMNOTIFY             = 0x0044 ,
    WM_WINDOWPOSCHANGING      = 0x0046,
    WM_WINDOWPOSCHANGED       = 0x0047,
    WM_POWER                  = 0x0048,
    WM_COPYDATA               = 0x004A,
    WM_CANCELJOURNAL          = 0x004B,
    WM_NOTIFY                 = 0x004E,
    WM_INPUTLANGCHANGEREQUEST = 0x0050,
    WM_INPUTLANGCHANGE        = 0x0051,
    WM_TCARD                  = 0x0052,
    WM_HELP                   = 0x0053,
    WM_USERCHANGED            = 0x0054,
    WM_NOTIFYFORMAT           = 0x0055,
    WM_CONTEXTMENU            = 0x007B,
    WM_STYLECHANGING          = 0x007C,
    WM_STYLECHANGED           = 0x007D,
    WM_DISPLAYCHANGE          = 0x007E,
    WM_GETICON                = 0x007F,
    WM_SETICON                = 0x0080,
    WM_NCCREATE               = 0x0081,
    WM_NCDESTROY              = 0x0082,
    WM_NCCALCSIZE             = 0x0083,
    WM_NCHITTEST              = 0x0084,
    WM_NCPAINT                = 0x0085,
    WM_NCACTIVATE             = 0x0086,
    WM_GETDLGCODE             = 0x0087,
    WM_SYNCPAINT              = 0x0088,
    WM_NCMOUSEMOVE            = 0x00A0,
    WM_NCLBUTTONDOWN          = 0x00A1,
    WM_NCLBUTTONUP            = 0x00A2,
    WM_NCLBUTTONDBLCLK        = 0x00A3,
    WM_NCRBUTTONDOWN          = 0x00A4,
    WM_NCRBUTTONUP            = 0x00A5,
    WM_NCRBUTTONDBLCLK        = 0x00A6,
    WM_NCMBUTTONDOWN          = 0x00A7,
    WM_NCMBUTTONUP            = 0x00A8,
    WM_NCMBUTTONDBLCLK        = 0x00A9,
    WM_NCXBUTTONDOWN          = 0x00AB,
    WM_NCXBUTTONUP            = 0x00AC,
    WM_KEYDOWN                = 0x0100,
    WM_KEYUP                  = 0x0101,
    WM_CHAR                   = 0x0102,
    WM_DEADCHAR               = 0x0103,
    WM_SYSKEYDOWN             = 0x0104,
    WM_SYSKEYUP               = 0x0105,
    WM_SYSCHAR                = 0x0106,
    WM_SYSDEADCHAR            = 0x0107,
    WM_KEYLAST                = 0x0108,
    WM_IME_STARTCOMPOSITION   = 0x010D,
    WM_IME_ENDCOMPOSITION     = 0x010E,
    WM_IME_COMPOSITION        = 0x010F,
    WM_IME_KEYLAST            = 0x010F,
    WM_INITDIALOG             = 0x0110,
    WM_COMMAND                = 0x0111,
    WM_SYSCOMMAND             = 0x0112,
    WM_TIMER                  = 0x0113,
    WM_HSCROLL                = 0x0114,
    WM_VSCROLL                = 0x0115,
    WM_INITMENU               = 0x0116,
    WM_INITMENUPOPUP          = 0x0117,
    WM_MENUSELECT             = 0x011F,
    WM_MENUCHAR               = 0x0120,
    WM_ENTERIDLE              = 0x0121,
    WM_MENURBUTTONUP          = 0x0122,
    WM_MENUDRAG               = 0x0123,
    WM_MENUGETOBJECT          = 0x0124,
    WM_UNINITMENUPOPUP        = 0x0125,
    WM_MENUCOMMAND            = 0x0126,
    WM_CTLCOLORMSGBOX         = 0x0132,
    WM_CTLCOLOREDIT           = 0x0133,
    WM_CTLCOLORLISTBOX        = 0x0134,
    WM_CTLCOLORBTN            = 0x0135,
    WM_CTLCOLORDLG            = 0x0136,
    WM_CTLCOLORSCROLLBAR      = 0x0137,
    WM_CTLCOLORSTATIC         = 0x0138,
    WM_MOUSEMOVE              = 0x0200,
    WM_LBUTTONDOWN            = 0x0201,
    WM_LBUTTONUP              = 0x0202,
    WM_LBUTTONDBLCLK          = 0x0203,
    WM_RBUTTONDOWN            = 0x0204,
    WM_RBUTTONUP              = 0x0205,
    WM_RBUTTONDBLCLK          = 0x0206,
    WM_MBUTTONDOWN            = 0x0207,
    WM_MBUTTONUP              = 0x0208,
    WM_MBUTTONDBLCLK          = 0x0209,
    WM_MOUSEWHEEL             = 0x020A,
    WM_XBUTTONDOWN            = 0x020B,
    WM_XBUTTONUP              = 0x020C,
    WM_XBUTTONDBLCLK          = 0x020D,
    WM_PARENTNOTIFY           = 0x0210,
    WM_ENTERMENULOOP          = 0x0211,
    WM_EXITMENULOOP           = 0x0212,
    WM_NEXTMENU               = 0x0213,
    WM_SIZING                 = 0x0214,
    WM_CAPTURECHANGED         = 0x0215,
    WM_MOVING                 = 0x0216,
    WM_DEVICECHANGE           = 0x0219,
    WM_MDICREATE              = 0x0220,
    WM_MDIDESTROY             = 0x0221,
    WM_MDIACTIVATE            = 0x0222,
    WM_MDIRESTORE             = 0x0223,
    WM_MDINEXT                = 0x0224,
    WM_MDIMAXIMIZE            = 0x0225,
    WM_MDITILE                = 0x0226,
    WM_MDICASCADE             = 0x0227,
    WM_MDIICONARRANGE         = 0x0228,
    WM_MDIGETACTIVE           = 0x0229,
    WM_MDISETMENU             = 0x0230,
    WM_ENTERSIZEMOVE          = 0x0231,
    WM_EXITSIZEMOVE           = 0x0232,
    WM_DROPFILES              = 0x0233,
    WM_MDIREFRESHMENU         = 0x0234,
    WM_IME_SETCONTEXT         = 0x0281,
    WM_IME_NOTIFY             = 0x0282,
    WM_IME_CONTROL            = 0x0283,
    WM_IME_COMPOSITIONFULL    = 0x0284,
    WM_IME_SELECT             = 0x0285,
    WM_IME_CHAR               = 0x0286,
    WM_IME_REQUEST            = 0x0288,
    WM_IME_KEYDOWN            = 0x0290,
    WM_IME_KEYUP              = 0x0291,
    WM_MOUSEHOVER             = 0x02A1,
    WM_MOUSELEAVE             = 0x02A3,
    WM_CUT                    = 0x0300,
    WM_COPY                   = 0x0301,
    WM_PASTE                  = 0x0302,
    WM_CLEAR                  = 0x0303,
    WM_UNDO                   = 0x0304,
    WM_RENDERFORMAT           = 0x0305,
    WM_RENDERALLFORMATS       = 0x0306,
    WM_DESTROYCLIPBOARD       = 0x0307,
    WM_DRAWCLIPBOARD          = 0x0308,
    WM_PAINTCLIPBOARD         = 0x0309,
    WM_VSCROLLCLIPBOARD       = 0x030A,
    WM_SIZECLIPBOARD          = 0x030B,
    WM_ASKCBFORMATNAME        = 0x030C,
    WM_CHANGECBCHAIN          = 0x030D,
    WM_HSCROLLCLIPBOARD       = 0x030E,
    WM_QUERYNEWPALETTE        = 0x030F,
    WM_PALETTEISCHANGING      = 0x0310,
    WM_PALETTECHANGED         = 0x0311,
    WM_HOTKEY                 = 0x0312,
    WM_PRINT                  = 0x0317,
    WM_PRINTCLIENT            = 0x0318,
    WM_HANDHELDFIRST          = 0x0358,
    WM_HANDHELDLAST           = 0x035F,
    WM_AFXFIRST               = 0x0360,
    WM_AFXLAST                = 0x037F,
    WM_PENWINFIRST            = 0x0380,
    WM_PENWINLAST             = 0x038F,
    WM_APP                    = 0x8000,
    WM_USER                   = 0x0400,

    CCM_FIRST = 0x2000,// Common control shared messages
    CCM_LAST =(CCM_FIRST + 0x200),
    CCM_SETBKCOLOR = (CCM_FIRST + 1),
    CCM_SETCOLORSCHEME = (CCM_FIRST + 2),
    CCM_GETCOLORSCHEME = (CCM_FIRST + 3),
    CCM_GETDROPTARGET = (CCM_FIRST + 4),
    CCM_SETUNICODEFORMAT = (CCM_FIRST + 5),
    CCM_GETUNICODEFORMAT = (CCM_FIRST + 6),
    CCM_SETVERSION = (CCM_FIRST + 0x7),
    CCM_GETVERSION = (CCM_FIRST + 0x8),
    CCM_SETNOTIFYWINDOW = (CCM_FIRST + 0x9),
    CCM_SETWINDOWTHEME = (CCM_FIRST + 0xb),
    CCM_DPISCALE = (CCM_FIRST + 0xc),

    RB_INSERTBANDA = (WM_USER + 1),
    RB_DELETEBAND = (WM_USER + 2),
    RB_GETBARINFO = (WM_USER + 3),
    RB_SETBARINFO = (WM_USER + 4),
    RB_GETBANDINFO = (WM_USER + 5),
    RB_SETBANDINFOA = (WM_USER + 6),
    RB_SETPARENT = (WM_USER + 7),
    RB_HITTEST = (WM_USER + 8),
    RB_GETRECT = (WM_USER + 9),
    RB_INSERTBANDW = (WM_USER + 10),
    RB_SETBANDINFOW = (WM_USER + 11),
    RB_GETBANDCOUNT = (WM_USER + 12),
    RB_GETROWCOUNT = (WM_USER + 13),
    RB_GETROWHEIGHT = (WM_USER + 14),
    RB_IDTOINDEX = (WM_USER + 16), 
    RB_GETTOOLTIPS = (WM_USER + 17),
    RB_SETTOOLTIPS = (WM_USER + 18),
    RB_SETBKCOLOR = (WM_USER + 19),
    RB_GETBKCOLOR = (WM_USER + 20),
    RB_SETTEXTCOLOR = (WM_USER + 21),
    RB_GETTEXTCOLOR = (WM_USER + 22),
    RB_SIZETORECT = (WM_USER + 23),
    RB_SETCOLORSCHEME = CCM_SETCOLORSCHEME,
    RB_GETCOLORSCHEME = CCM_GETCOLORSCHEME,
    RB_BEGINDRAG = (WM_USER + 24),
    RB_ENDDRAG = (WM_USER + 25),
    RB_DRAGMOVE = (WM_USER + 26),
    RB_GETBARHEIGHT = (WM_USER + 27),
    RB_GETBANDINFOW = (WM_USER + 28),
    RB_GETBANDINFOA = (WM_USER + 29),
    RB_MINIMIZEBAND = (WM_USER + 30),
    RB_MAXIMIZEBAND = (WM_USER + 31),
    RB_GETDROPTARGET = (CCM_GETDROPTARGET),
    RB_GETBANDBORDERS = (WM_USER + 34), 
    RB_SHOWBAND = (WM_USER + 35), 
    RB_SETPALETTE = (WM_USER + 37),
    RB_GETPALETTE = (WM_USER + 38),
    RB_MOVEBAND = (WM_USER + 39),
    RB_SETUNICODEFORMAT = CCM_SETUNICODEFORMAT,
    RB_GETUNICODEFORMAT = CCM_GETUNICODEFORMAT,
    RB_GETBANDMARGINS = (WM_USER + 40),
    RB_SETWINDOWTHEME = CCM_SETWINDOWTHEME,
    RB_PUSHCHEVRON = (WM_USER + 43),
  } // enum Msgs

  #endregion

  #region Cursors

  public enum Cursors : uint
  {
    IDC_ARROW		    = 32512U,
    IDC_IBEAM       = 32513U,
    IDC_WAIT        = 32514U,
    IDC_CROSS       = 32515U,
    IDC_UPARROW     = 32516U,
    IDC_SIZE        = 32640U,
    IDC_ICON        = 32641U,
    IDC_SIZENWSE    = 32642U,
    IDC_SIZENESW    = 32643U,
    IDC_SIZEWE      = 32644U,
    IDC_SIZENS      = 32645U,
    IDC_SIZEALL     = 32646U,
    IDC_NO          = 32648U,
    IDC_HAND        = 32649U,
    IDC_APPSTARTING = 32650U,
    IDC_HELP        = 32651U
  } // enum Cursors 

  #endregion
 
  #region MouseActivateFlags

  public enum MouseActivateFlags
  {
    MA_ACTIVATE			= 1,
    MA_ACTIVATEANDEAT   = 2,
    MA_NOACTIVATE       = 3,
    MA_NOACTIVATEANDEAT = 4
  } // enum MouseActivateFlags

  #endregion  
 
  #region AnimateFlags
  
  public enum AnimateFlags
  {
    AW_HOR_POSITIVE = 0x00000001,
    AW_HOR_NEGATIVE = 0x00000002,
    AW_VER_POSITIVE = 0x00000004,
    AW_VER_NEGATIVE = 0x00000008,
    AW_CENTER		    = 0x00000010,
    AW_HIDE			    = 0x00010000,
    AW_ACTIVATE		  = 0x00020000,
    AW_SLIDE		    = 0x00040000,
    AW_BLEND		    = 0x00080000
  } // enum AnimateFlags

  #endregion

  #region CommonObjectIDs
 
  public enum CommonObjectIDs : uint
  {
    CHILDID_SELF    = 0,
    OBJID_WINDOW    = 0x00000000,
    OBJID_SYSMENU   = 0xFFFFFFFF,
    OBJID_TITLEBAR  = 0xFFFFFFFE,
    OBJID_MENU      = 0xFFFFFFFD,
    OBJID_CLIENT    = 0xFFFFFFFC,
    OBJID_VSCROLL   = 0xFFFFFFFB,
    OBJID_HSCROLL   = 0xFFFFFFFA,
    OBJID_SIZEGRIP  = 0xFFFFFFF9,
    OBJID_CARET     = 0xFFFFFFF8,
    OBJID_CURSOR    = 0xFFFFFFF7,
    OBJID_ALERT     = 0xFFFFFFF6,
    OBJID_SOUND     = 0xFFFFFFF5,
    CCHILDREN_FRAME =  7,
  } // enum CommonObjectIDs 

  #endregion

  #region BrowseFlags

  [ Flags ]
  public enum BrowseFlags
  {
    BIF_DEFAULT				      = 0x0000,
    BIF_BROWSEFORCOMPUTER	  =	0x1000,
    BIF_BROWSEFORPRINTER	  =	0x2000,
    BIF_BROWSEINCLUDEFILES	=	0x4000,
    BIF_BROWSEINCLUDEURLS	  =	0x0080,
    BIF_DONTGOBELOWDOMAIN	  =	0x0002,
    BIF_EDITBOX				      =	0x0010,
    BIF_NEWDIALOGSTYLE		  =	0x0040,
    BIF_NONEWFOLDERBUTTON	  =	0x0200,
    BIF_RETURNFSANCESTORS	  =	0x0008,
    BIF_RETURNONLYFSDIRS	  =	0x0001,
    BIF_SHAREABLE			      =	0x8000,
    BIF_STATUSTEXT			    =	0x0004,
    BIF_UAHINT				      =	0x0100,
    BIF_VALIDATE			      =	0x0020,
    BIF_NOTRANSLATETARGETS  =	0x0400,
  } // enum BrowseFlags

  #endregion

  #region SystemMetricsFlags

  public enum SystemMetricsFlags
  {
    SM_CXSCREEN             = 0,
    SM_CYSCREEN             = 1,
    SM_CXVSCROLL            = 2,
    SM_CYHSCROLL            = 3,
    SM_CYCAPTION            = 4,
    SM_CXBORDER             = 5,
    SM_CYBORDER             = 6,
    SM_CXDLGFRAME           = 7,
    SM_CYDLGFRAME           = 8,
    SM_CYVTHUMB             = 9,
    SM_CXHTHUMB             = 10,
    SM_CXICON               = 11,
    SM_CYICON               = 12,
    SM_CXCURSOR             = 13,
    SM_CYCURSOR             = 14,
    SM_CYMENU               = 15,
    SM_CXFULLSCREEN         = 16,
    SM_CYFULLSCREEN         = 17,
    SM_CYKANJIWINDOW        = 18,
    SM_MOUSEPRESENT         = 19,
    SM_CYVSCROLL            = 20,
    SM_CXHSCROLL            = 21,
    SM_DEBUG                = 22,
    SM_SWAPBUTTON           = 23,
    SM_RESERVED1            = 24,
    SM_RESERVED2            = 25,
    SM_RESERVED3            = 26,
    SM_RESERVED4            = 27,
    SM_CXMIN                = 28,
    SM_CYMIN                = 29,
    SM_CXSIZE               = 30,
    SM_CYSIZE               = 31,
    SM_CXFRAME              = 32,
    SM_CYFRAME              = 33,
    SM_CXMINTRACK           = 34,
    SM_CYMINTRACK           = 35,
    SM_CXDOUBLECLK          = 36,
    SM_CYDOUBLECLK          = 37,
    SM_CXICONSPACING        = 38,
    SM_CYICONSPACING        = 39,
    SM_MENUDROPALIGNMENT    = 40,
    SM_PENWINDOWS           = 41,
    SM_DBCSENABLED          = 42,
    SM_CMOUSEBUTTONS        = 43,
    SM_CXFIXEDFRAME         = SM_CXDLGFRAME,
    SM_CYFIXEDFRAME         = SM_CYDLGFRAME,
    SM_CXSIZEFRAME          = SM_CXFRAME,
    SM_CYSIZEFRAME          = SM_CYFRAME,
    SM_SECURE               = 44,
    SM_CXEDGE               = 45,
    SM_CYEDGE               = 46,
    SM_CXMINSPACING         = 47,
    SM_CYMINSPACING         = 48,
    SM_CXSMICON             = 49,
    SM_CYSMICON             = 50,
    SM_CYSMCAPTION          = 51,
    SM_CXSMSIZE             = 52,
    SM_CYSMSIZE             = 53,
    SM_CXMENUSIZE           = 54,
    SM_CYMENUSIZE           = 55,
    SM_ARRANGE              = 56,
    SM_CXMINIMIZED          = 57,
    SM_CYMINIMIZED          = 58,
    SM_CXMAXTRACK           = 59,
    SM_CYMAXTRACK           = 60,
    SM_CXMAXIMIZED          = 61,
    SM_CYMAXIMIZED          = 62,
    SM_NETWORK              = 63,
    SM_CLEANBOOT            = 67,
    SM_CXDRAG               = 68,
    SM_CYDRAG               = 69,
    SM_SHOWSOUNDS           = 70,
    SM_CXMENUCHECK          = 71,
    SM_CYMENUCHECK          = 72,
    SM_SLOWMACHINE          = 73,
    SM_MIDEASTENABLED       = 74,
    SM_MOUSEWHEELPRESENT    = 75,
    SM_XVIRTUALSCREEN       = 76,
    SM_YVIRTUALSCREEN       = 77,
    SM_CXVIRTUALSCREEN      = 78,
    SM_CYVIRTUALSCREEN      = 79,
    SM_CMONITORS            = 80,
    SM_SAMEDISPLAYFORMAT    = 81,
    SM_IMMENABLED           = 82,
    SM_CXFOCUSBORDER        = 83,
    SM_CYFOCUSBORDER        = 84,
  } // enum SystemMetricsFlags

  #endregion

  #region RedrawWindowFlags

  public enum RedrawWindowFlags
  {
    RDW_INVALIDATE          = 0x0001,
    RDW_INTERNALPAINT       = 0x0002,
    RDW_ERASE               = 0x0004,
    RDW_VALIDATE            = 0x0008,
    RDW_NOINTERNALPAINT     = 0x0010,
    RDW_NOERASE             = 0x0020,
    RDW_NOCHILDREN          = 0x0040,
    RDW_ALLCHILDREN         = 0x0080,
    RDW_UPDATENOW           = 0x0100,
    RDW_ERASENOW            = 0x0200,
    RDW_FRAME               = 0x0400,
    RDW_NOFRAME             = 0x0800,
  } // enum RedrawWindowFlags

  #endregion

  #region RebarBandStyles
  
  [Flags] 
  public enum RebarBandStyles: uint
  {
    RBBS_BREAK = 0x00000001, // break to new line
    RBBS_FIXEDSIZE = 0x00000002, // band can't be sized
    RBBS_CHILDEDGE = 0x00000004, // edge around top & bottom of child window
    RBBS_HIDDEN = 0x00000008, // don't show
    RBBS_NOVERT = 0x00000010, // don't show when vertical
    RBBS_FIXEDBMP = 0x00000020, // bitmap doesn't move during band resize
    RBBS_VARIABLEHEIGHT = 0x00000040, // allow autosizing of this child vertically
    RBBS_GRIPPERALWAYS = 0x00000080, // always show the gripper
    RBBS_NOGRIPPER = 0x00000100, // never show the gripper
    RBBS_USECHEVRON = 0x00000200, // display drop-down button for this band if it's sized smaller than ideal width
    RBBS_HIDETITLE = 0x00000400, // keep band title hidden
    RBBS_TOPALIGN = 0x00000800 // keep band title hidden
  }

  #endregion

  #region RebarBandInfoFlags
  
  [Flags]
  public enum RebarBandInfoFlags: uint
  {
    RBBIM_STYLE        = 0x00000001,
    RBBIM_COLORS       = 0x00000002,
    RBBIM_TEXT         = 0x00000004,
    RBBIM_IMAGE        = 0x00000008,
    RBBIM_CHILD        = 0x00000010,
    RBBIM_CHILDSIZE    = 0x00000020,
    RBBIM_SIZE         = 0x00000040,
    RBBIM_BACKGROUND   = 0x00000080,
    RBBIM_ID           = 0x00000100,
    RBBIM_IDEALSIZE    = 0x00000200,
    RBBIM_LPARAM       = 0x00000400,
    RBBIM_HEADERSIZE   = 0x00000800  // control the size of the header
  }

  #endregion
  
  #region RebarImageListFlags
  
  [Flags]
  public enum RebarImageListFlags: uint
  {
    RBIM_IMAGELIST = 0x00000001
  }

  #endregion

  #region RebarSizeToRectFlags

  [Flags]
  public enum RebarSizeToRectFlags: uint
  {
    RBSTR_CHANGERECT = 0x0001   // flags for RB_SIZETORECT
  }

  #endregion

  #region ColorConstants
  
  public enum ColorConstants: uint 
  {
    CLR_NONE = 0xFFFFFFFFU,
    CLR_DEFAULT = 0xFF000000U
  }

  #endregion

  #region WindowLongConstants
  
  public enum WindowLongConstants: int
  {
    GWL_WNDPROC        = (-4),
    GWL_HINSTANCE      = (-6),
    GWL_HWNDPARENT     = (-8),
    GWL_STYLE          = (-16),
    GWL_EXSTYLE        = (-20),
    GWL_USERDATA       = (-21),
    GWL_ID             = (-12),
    GWLP_WNDPROC       = (-4),
    GWLP_HINSTANCE     = (-6),
    GWLP_HWNDPARENT    = (-8),
    GWLP_USERDATA      = (-21),
    GWLP_ID            = (-12),
  }

  #endregion

  #region InitWindowsCommonControlsConstants

  public enum InitWindowsCommonControlsConstants: int
  {
    ICC_LISTVIEW_CLASSES = 0x00000001, // listview, header
    ICC_TREEVIEW_CLASSES = 0x00000002, // treeview, tooltips
    ICC_BAR_CLASSES = 0x00000004, // toolbar, statusbar, trackbar, tooltips
    ICC_TAB_CLASSES = 0x00000008, // tab, tooltips
    ICC_UPDOWN_CLASS = 0x00000010, // updown
    ICC_PROGRESS_CLASS = 0x00000020, // progress
    ICC_HOTKEY_CLASS = 0x00000040, // hotkey
    ICC_ANIMATE_CLASS = 0x00000080, // animate
    ICC_WIN95_CLASSES = 0x000000FF,
    ICC_DATE_CLASSES = 0x00000100, // month picker, date picker, time picker, updown
    ICC_USEREX_CLASSES = 0x00000200, // comboex
    ICC_COOL_CLASSES = 0x00000400, // rebar (coolbar) control
    ICC_INTERNET_CLASSES = 0x00000800,
    ICC_PAGESCROLLER_CLASS = 0x00001000, // page scroller
    ICC_NATIVEFNTCTL_CLASS = 0x00002000, // native font control
    ICC_STANDARD_CLASSES = 0x00004000,
    ICC_LINK_CLASS = 0x00008000,
  }

  #endregion

  #region WindowsNotifyConstants


  public enum WindowsNotifyConstants: int
  {
    NM_FIRST              = (0-  0),       // generic to all controls
    NM_LAST               = (0- 99),
    LVN_FIRST             = (0-100),       // listview
    LVN_LAST              = (0-199),
    HDN_FIRST             = (0-300),       // header
    HDN_LAST              = (0-399),
    TVN_FIRST             = (0-400),       // treeview
    TVN_LAST              = (0-499),
    TTN_FIRST             = (0-520),       // tooltips
    TTN_LAST              = (0-549),
    TCN_FIRST             = (0-550),       // tab control
    TCN_LAST              = (0-580),
    CDN_FIRST             = (0-601),       // common dialog (new)
    CDN_LAST              = (0-699),
    TBN_FIRST             = (0-700),       // toolbar
    TBN_LAST              = (0-720),
    UDN_FIRST             = (0-721),        // updown
    UDN_LAST              = (0-740),
    MCN_FIRST             = (0-750),       // monthcal
    MCN_LAST              = (0-759),
    DTN_FIRST             = (0-760),       // datetimepick
    DTN_LAST              = (0-799),
    CBEN_FIRST            = (0-800),       // combo box ex
    CBEN_LAST             = (0-830),
    RBN_FIRST             = (0-831),       // rebar
    RBN_LAST              = (0-859),
    IPN_FIRST             = (0-860),       // internet address
    IPN_LAST              = (0-879),       // internet address
    SBN_FIRST             = (0-880),       // status bar
    SBN_LAST              = (0-899),
    PGN_FIRST             = (0-900),       // Pager Control
    PGN_LAST              = (0-950),
    WMN_FIRST             = (0-1000),
    WMN_LAST              = (0-1200),
    BCN_FIRST             = (0-1250),
    BCN_LAST              = (0-1350),
    NM_OUTOFMEMORY         = (NM_FIRST-1),
    NM_CLICK               = (NM_FIRST-2),    // uses NMCLICK struct
    NM_DBLCLK              = (NM_FIRST-3),
    NM_RETURN              = (NM_FIRST-4),
    NM_RCLICK              = (NM_FIRST-5),    // uses NMCLICK struct
    NM_RDBLCLK             = (NM_FIRST-6),
    NM_SETFOCUS            = (NM_FIRST-7),
    NM_KILLFOCUS           = (NM_FIRST-8),
    NM_CUSTOMDRAW          = (NM_FIRST-12),
    NM_HOVER               = (NM_FIRST-13),
    NM_NCHITTEST           = (NM_FIRST-14),   // uses NMMOUSE struct
    NM_KEYDOWN             = (NM_FIRST-15),   // uses NMKEY struct
    NM_RELEASEDCAPTURE     = (NM_FIRST-16),
    NM_SETCURSOR           = (NM_FIRST-17),   // uses NMMOUSE struct
    NM_CHAR                = (NM_FIRST-18),   // uses NMCHAR struct
    NM_TOOLTIPSCREATED     = (NM_FIRST-19),   // notify of when the tooltips window is create
    NM_LDOWN               = (NM_FIRST-20),
    NM_RDOWN               = (NM_FIRST-21),
    NM_THEMECHANGED        = (NM_FIRST-22),
    RBN_HEIGHTCHANGE   = (RBN_FIRST - 0),
    RBN_GETOBJECT      = (RBN_FIRST - 1),
    RBN_LAYOUTCHANGED  = (RBN_FIRST - 2),
    RBN_AUTOSIZE       = (RBN_FIRST - 3),
    RBN_BEGINDRAG      = (RBN_FIRST - 4),
    RBN_ENDDRAG         = (RBN_FIRST - 5),
    RBN_DELETINGBAND   = (RBN_FIRST - 6),     // Uses NMREBAR
    RBN_DELETEDBAND    = (RBN_FIRST - 7),     // Uses NMREBAR
    RBN_CHILDSIZE      = (RBN_FIRST - 8),
    RBN_CHEVRONPUSHED  = (RBN_FIRST - 10),
    RBN_MINMAX         = (RBN_FIRST - 21),
    RBN_AUTOBREAK      = (RBN_FIRST - 22),
  }

  #endregion

  #region WindowMessages

  /// <summary>
  /// Window messages 
  /// </summary>
  public enum WindowMessages
  {
    WM_APP = 32768,
    WM_ACTIVATE = 6,
    WM_ACTIVATEAPP = 28,
    WM_AFXFIRST = 864,
    WM_AFXLAST = 895,
    WM_ASKCBFORMATNAME = 780,
    WM_CANCELJOURNAL = 75,
    WM_CANCELMODE = 31,
    WM_CAPTURECHANGED = 533,
    WM_CHANGECBCHAIN = 781,
    WM_CHAR = 258,
    WM_CHARTOITEM = 47,
    WM_CHILDACTIVATE = 34,
    WM_CLEAR = 771,
    WM_CLOSE = 16,
    WM_COMMAND = 273,
    WM_COMMNOTIFY = 68,
    WM_COMPACTING = 65,
    WM_COMPAREITEM = 57,
    WM_CONTEXTMENU = 123,
    WM_COPY = 769,
    WM_COPYDATA = 74,
    WM_CREATE = 1,
    WM_CTLCOLOR = 0x0019,
    WM_CTLCOLORBTN = 309,
    WM_CTLCOLORDLG = 310,
    WM_CTLCOLOREDIT = 307,
    WM_CTLCOLORLISTBOX = 308,
    WM_CTLCOLORMSGBOX = 306,
    WM_CTLCOLORSCROLLBAR = 311,
    WM_CTLCOLORSTATIC = 312,
    WM_CUT = 768,
    WM_DEADCHAR = 259,
    WM_DELETEITEM = 45,
    WM_DESTROY = 2,
    WM_DESTROYCLIPBOARD = 775,
    WM_DEVICECHANGE = 537,
    WM_DEVMODECHANGE = 27,
    WM_DISPLAYCHANGE = 126,
    WM_DRAWCLIPBOARD = 776,
    WM_DRAWITEM = 43,
    WM_DROPFILES = 563,
    WM_ENABLE = 10,
    WM_ENDSESSION = 22,
    WM_ENTERIDLE = 289,
    WM_ENTERMENULOOP = 529,
    WM_ENTERSIZEMOVE = 561,
    WM_ERASEBKGND = 20,
    WM_EXITMENULOOP = 530,
    WM_EXITSIZEMOVE = 562,
    WM_FONTCHANGE = 29,
    WM_GETDLGCODE = 135,
    WM_GETFONT = 49,
    WM_GETHOTKEY = 51,
    WM_GETICON = 127,
    WM_GETMINMAXINFO = 36,
    WM_GETTEXT = 13,
    WM_GETTEXTLENGTH = 14,
    WM_HANDHELDFIRST = 856,
    WM_HANDHELDLAST = 863,
    WM_HELP = 83,
    WM_HOTKEY = 786,
    WM_HSCROLL = 276,
    WM_HSCROLLCLIPBOARD = 782,
    WM_ICONERASEBKGND = 39,
    WM_INITDIALOG = 272,
    WM_INITMENU = 278,
    WM_INITMENUPOPUP = 279,
    WM_UNINITMENUPOPUP = 293,
    WM_INPUTLANGCHANGE = 81,
    WM_INPUTLANGCHANGEREQUEST = 80,
    WM_KEYDOWN = 256,
    WM_KEYUP = 257,
    WM_KILLFOCUS = 8,
    WM_MDIACTIVATE = 546,
    WM_MDICASCADE = 551,
    WM_MDICREATE = 544,
    WM_MDIDESTROY = 545,
    WM_MDIGETACTIVE = 553,
    WM_MDIICONARRANGE = 552,
    WM_MDIMAXIMIZE = 549,
    WM_MDINEXT = 548,
    WM_MDIREFRESHMENU = 564,
    WM_MDIRESTORE = 547,
    WM_MDISETMENU = 560,
    WM_MDITILE = 550,
    WM_MEASUREITEM = 44,
    WM_MENUCHAR = 288,
    WM_MENUSELECT = 287,
    WM_MENUCOMMAND = 294,
    WM_NEXTMENU = 531,
    WM_MOVE = 3,
    WM_MOVING = 534,
    WM_NCACTIVATE = 134,
    WM_NCCALCSIZE = 131,
    WM_NCCREATE = 129,
    WM_NCDESTROY = 130,
    WM_NCHITTEST = 132,
    WM_NCLBUTTONDBLCLK = 163,
    WM_NCLBUTTONDOWN = 161,
    WM_NCLBUTTONUP = 162,
    WM_NCMBUTTONDBLCLK = 169,
    WM_NCMBUTTONDOWN = 167,
    WM_NCMBUTTONUP = 168,
    WM_NCMOUSEMOVE = 160,
    WM_NCPAINT = 133,
    WM_NCRBUTTONDBLCLK = 166,
    WM_NCRBUTTONDOWN = 164,
    WM_NCRBUTTONUP = 165,
    WM_NEXTDLGCTL = 40,
    WM_NOTIFY = 78,
    WM_NOTIFYFORMAT = 85,
    WM_NULL = 0,
    WM_PAINT = 15,
    WM_PAINTCLIPBOARD = 777,
    WM_PAINTICON = 38,
    WM_PALETTECHANGED = 785,
    WM_PALETTEISCHANGING = 784,
    WM_PARENTNOTIFY = 528,
    WM_PASTE = 770,
    WM_PENWINFIRST = 896,
    WM_PENWINLAST = 911,
    WM_POWER = 72,
    WM_POWERBROADCAST = 536,
    WM_PRINT = 791,
    WM_PRINTCLIENT = 792,
    WM_QUERYDRAGICON = 55,
    WM_QUERYENDSESSION = 17,
    WM_QUERYNEWPALETTE = 783,
    WM_QUERYOPEN = 19,
    WM_QUEUESYNC = 35,
    WM_QUIT = 18,
    WM_RENDERALLFORMATS = 774,
    WM_RENDERFORMAT = 773,
    WM_SETCURSOR = 32,
    WM_SETFOCUS = 7,
    WM_SETFONT = 48,
    WM_SETHOTKEY = 50,
    WM_SETICON = 128,
    WM_SETREDRAW = 11,
    WM_SETTEXT = 12,
    WM_SETTINGCHANGE = 26,
    WM_SHOWWINDOW = 24,
    WM_SIZE = 5,
    WM_SIZECLIPBOARD = 779,
    WM_SIZING = 532,
    WM_SPOOLERSTATUS = 42,
    WM_STYLECHANGED = 125,
    WM_STYLECHANGING = 124,
    WM_SYSCHAR = 262,
    WM_SYSCOLORCHANGE = 21,
    WM_SYSCOMMAND = 274,
    WM_SYSDEADCHAR = 263,
    WM_SYSKEYDOWN = 260,
    WM_SYSKEYUP = 261,
    WM_TCARD = 82,
    WM_TIMECHANGE = 30,
    WM_TIMER = 275,
    WM_UNDO = 772,
    WM_USER = 1024,
    WM_USERCHANGED = 84,
    WM_VKEYTOITEM = 46,
    WM_VSCROLL = 277,
    WM_VSCROLLCLIPBOARD = 778,
    WM_WINDOWPOSCHANGED = 71,
    WM_WINDOWPOSCHANGING = 70,
    WM_WININICHANGE = 26,
    WM_KEYFIRST = 256,
    WM_KEYLAST = 264,
    WM_SYNCPAINT = 136,
    WM_MOUSEACTIVATE = 33,
    WM_MOUSEMOVE = 512,
    WM_LBUTTONDOWN = 513,
    WM_LBUTTONUP = 514,
    WM_LBUTTONDBLCLK = 515,
    WM_RBUTTONDOWN = 516,
    WM_RBUTTONUP = 517,
    WM_RBUTTONDBLCLK = 518,
    WM_MBUTTONDOWN = 519,
    WM_MBUTTONUP = 520,
    WM_MBUTTONDBLCLK = 521,
    WM_MOUSEWHEEL = 522,
    WM_MOUSEFIRST = 512,
    WM_MOUSELAST = 522,
    WM_MOUSEHOVER = 0x2A1,
    WM_MOUSELEAVE = 0x2A3,
    WM_SHNOTIFY = 0x0401,
    WM_UNICHAR = 0x0109,
    WM_THEMECHANGED = 0x031A,
  } // enum WindowsMessages

  #endregion

  #region KeyStatesMasks

  /// <summary>
  /// Key State Masks / MK
  /// </summary>
  public enum KeyStatesMasks
  {
    MK_LBUTTON          = 0x0001,
    MK_RBUTTON          = 0x0002,
    MK_SHIFT            = 0x0004,
    MK_CONTROL          = 0x0008,
    MK_MBUTTON          = 0x0010,
    MK_XBUTTON1         = 0x0020,
    MK_XBUTTON2         = 0x0040,
  } // enum KeyStatesMasks

  #endregion

  #region EditControlNotificationCodes

  /// <summary>
  /// Edit Control Notification Codes / EN
  /// </summary>
  public enum EditControlNotificationCodes
  {
    EN_SETFOCUS         = 0x0100,
    EN_KILLFOCUS        = 0x0200,
    EN_CHANGE           = 0x0300,
    EN_UPDATE           = 0x0400,
    EN_ERRSPACE         = 0x0500,
    EN_MAXTEXT          = 0x0501,
    EN_HSCROLL          = 0x0601,
    EN_VSCROLL          = 0x0602,
    EN_ALIGN_LTR_EC     = 0x0700,
    EN_ALIGN_RTL_EC     = 0x0701,
  } // enum EditControlNotificationCodes

  #endregion

  #region ComboBoxNotificationCodes

  /// <summary>
  /// Combo Box Notification Codes / CBN
  /// </summary>
  public enum ComboBoxNotificationCodes
  {
    CBN_ERRSPACE        = (-1),
    CBN_SELCHANGE       = 1,
    CBN_DBLCLK          = 2,
    CBN_SETFOCUS        = 3,
    CBN_KILLFOCUS       = 4,
    CBN_EDITCHANGE      = 5,
    CBN_EDITUPDATE      = 6,
    CBN_DROPDOWN        = 7,
    CBN_CLOSEUP         = 8,
    CBN_SELENDOK        = 9,
    CBN_SELENDCANCEL    = 10,
  } // enum ComboBoxNotificationCodes

  #endregion

  #region ComboBoxMessages 

  /// <summary>
  /// Combo Box Messages / CB
  /// </summary>
  public enum ComboBoxMessages
  {
    CB_GETEDITSEL               = 0x0140,
    CB_LIMITTEXT                = 0x0141,
    CB_SETEDITSEL               = 0x0142,
    CB_ADDSTRING                = 0x0143,
    CB_DELETESTRING             = 0x0144,
    CB_DIR                      = 0x0145,
    CB_GETCOUNT                 = 0x0146,
    CB_GETCURSEL                = 0x0147,
    CB_GETLBTEXT                = 0x0148,
    CB_GETLBTEXTLEN             = 0x0149,
    CB_INSERTSTRING             = 0x014A,
    CB_RESETCONTENT             = 0x014B,
    CB_FINDSTRING               = 0x014C,
    CB_SELECTSTRING             = 0x014D,
    CB_SETCURSEL                = 0x014E,
    CB_SHOWDROPDOWN             = 0x014F,
    CB_GETITEMDATA              = 0x0150,
    CB_SETITEMDATA              = 0x0151,
    CB_GETDROPPEDCONTROLRECT    = 0x0152,
    CB_SETITEMHEIGHT            = 0x0153,
    CB_GETITEMHEIGHT            = 0x0154,
    CB_SETEXTENDEDUI            = 0x0155,
    CB_GETEXTENDEDUI            = 0x0156,
    CB_GETDROPPEDSTATE          = 0x0157,
    CB_FINDSTRINGEXACT          = 0x0158,
    CB_SETLOCALE                = 0x0159,
    CB_GETLOCALE                = 0x015A,
    CB_GETTOPINDEX              = 0x015b,
    CB_SETTOPINDEX              = 0x015c,
    CB_GETHORIZONTALEXTENT      = 0x015d,
    CB_SETHORIZONTALEXTENT      = 0x015e,
    CB_GETDROPPEDWIDTH          = 0x015f,
    CB_SETDROPPEDWIDTH          = 0x0160,
    CB_INITSTORAGE              = 0x0161,
    CB_MULTIPLEADDSTRING        = 0x0163,
    CB_GETCOMBOBOXINFO          = 0x0164,
    CB_FIRST                    = 0x1700,
    CB_SETMINVISIBLE            = CB_FIRST + 1,
    CB_GETMINVISIBLE            = CB_FIRST + 2,
  } // enum ComboBoxMessages

  #endregion

  #region ScrollBarFlags

  /// <summary>
  /// ScrollBar flags 
  /// </summary>
  public enum ScrollBarFlags
  {
    /// <summary>
    /// Scrolls one line up
    /// </summary>
    SB_LINEUP = 0,
    /// <summary>
    /// Scrolls one line left
    /// </summary>
    SB_LINELEFT = 0,
    /// <summary>
    /// Scrolls one line down
    /// </summary>
    SB_LINEDOWN = 1,
    /// <summary>
    /// Scrolls one page right
    /// </summary>
    SB_LINERIGHT = 1,
    /// <summary>
    /// Scrolls one page up
    /// </summary>
    SB_PAGEUP = 2,
    /// <summary>
    /// Scrolls one page left
    /// </summary>
    SB_PAGELEFT = 2,
    /// <summary>
    /// Scrolls one page down
    /// </summary>
    SB_PAGEDOWN = 3,
    /// <summary>
    /// Scrolls one page right
    /// </summary>
    SB_PAGERIGHT = 3,
    /// <summary>
    /// Scrolls to the upper left
    /// </summary>
    SB_TOP = 6,
    /// <summary>
    /// Scrolls to the lower right
    /// </summary>
    SB_BOTTOM = 7,
    /// <summary>
    /// Ends scroll
    /// </summary>
    SB_ENDSCROLL = 8,
  } // enum ScrollBarFlags

  #endregion

  #region EditControlMessages

  /// <summary>
  /// Edit Control Messages
  /// </summary>
  public enum EditControlMessages
  {
    EM_GETSEL               = 0x00B0,
    EM_SETSEL               = 0x00B1,
    EM_GETRECT              = 0x00B2,
    EM_SETRECT              = 0x00B3,
    EM_SETRECTNP            = 0x00B4,
    EM_SCROLL               = 0x00B5,
    EM_LINESCROLL           = 0x00B6,
    EM_SCROLLCARET          = 0x00B7,
    EM_GETMODIFY            = 0x00B8,
    EM_SETMODIFY            = 0x00B9,
    EM_GETLINECOUNT         = 0x00BA,
    EM_LINEINDEX            = 0x00BB,
    EM_SETHANDLE            = 0x00BC,
    EM_GETHANDLE            = 0x00BD,
    EM_GETTHUMB             = 0x00BE,
    EM_LINELENGTH           = 0x00C1,
    EM_REPLACESEL           = 0x00C2,
    EM_GETLINE              = 0x00C4,
    EM_LIMITTEXT            = 0x00C5,
    EM_CANUNDO              = 0x00C6,
    EM_UNDO                 = 0x00C7,
    EM_FMTLINES             = 0x00C8,
    EM_LINEFROMCHAR         = 0x00C9,
    EM_SETTABSTOPS          = 0x00CB,
    EM_SETPASSWORDCHAR      = 0x00CC,
    EM_EMPTYUNDOBUFFER      = 0x00CD,
    EM_GETFIRSTVISIBLELINE  = 0x00CE,
    EM_SETREADONLY          = 0x00CF,
    EM_SETWORDBREAKPROC     = 0x00D0,
    EM_GETWORDBREAKPROC     = 0x00D1,
    EM_GETPASSWORDCHAR      = 0x00D2,
    EM_SETMARGINS           = 0x00D3,
    EM_GETMARGINS           = 0x00D4,
    EM_SETLIMITTEXT         = EM_LIMITTEXT,
    EM_GETLIMITTEXT         = 0x00D5,
    EM_POSFROMCHAR          = 0x00D6,
    EM_CHARFROMPOS          = 0x00D7,
    EM_SETIMESTATUS         = 0x00D8,
    EM_GETIMESTATUS         = 0x00D9,
  } // enum EditControlMessages

  #endregion

  #region MenuItemMasks

  /// <summary>
  /// MenuItem Masks / MIIM
  /// </summary>
  public	enum MenuItemMasks : uint
  {
    MIIM_STATE =			0x00000001,
    MIIM_ID =	        0x00000002,
    MIIM_SUBMENU	=		0x00000004,
    MIIM_CHECKMARKS =	0x00000008,
    MIIM_TYPE =			0x00000010,
    MIIM_DATA =			0x00000020,
    MIIM_STRING =		0x00000040,
    MIIM_BITMAP =		0x00000080,
    MIIM_FTYPE =			0x00000100
  } // enum MenuItemMasks

  #endregion

  #region MenuItemFlags

  /// <summary>
  /// MenuItem Flags 
  /// </summary>
  public	enum	MenuItemFlags : uint
  {
    MF_INSERT =        0x00000000,
    MF_CHANGE =        0x00000080,
    MF_APPEND =        0x00000100,
    MF_DELETE =        0x00000200,
    MF_REMOVE =        0x00001000,
    MF_BYCOMMAND =     0x00000000,
    MF_BYPOSITION =    0x00000400,
    MF_SEPARATOR =     0x00000800,
    MF_ENABLED =       0x00000000,
    MF_GRAYED =        0x00000001,
    MF_DISABLED =      0x00000002,
    MF_UNCHECKED =     0x00000000,
    MF_CHECKED =       0x00000008,
    MF_USECHECKBITMAPS=0x00000200,
    MF_STRING =        0x00000000,
    MF_BITMAP =        0x00000004,
    MF_OWNERDRAW =     0x00000100,
    MF_POPUP =         0x00000010,
    MF_MENUBARBREAK =  0x00000020,
    MF_MENUBREAK =     0x00000040,
    MF_UNHILITE =      0x00000000,
    MF_HILITE =        0x00000080,
    MF_DEFAULT =       0x00001000,
    MF_SYSMENU =       0x00002000,
    MF_HELP =          0x00004000,
    MF_RIGHTJUSTIFY =  0x00004000,
    MF_MOUSESELECT =   0x00008000
  } // enum	MenuItemFlags

  #endregion

  #region MenuItemStates

  /// <summary>
  /// MenuItem States
  /// </summary>
  public	enum MenuItemStates : uint
  {
    MFS_GRAYED =        0x00000003,
    MFS_DISABLED =      MFS_GRAYED,
    MFS_CHECKED =       MenuItemFlags.MF_CHECKED,
    MFS_HILITE =        MenuItemFlags.MF_HILITE,
    MFS_ENABLED =       MenuItemFlags.MF_ENABLED,
    MFS_UNCHECKED =     MenuItemFlags.MF_UNCHECKED,
    MFS_UNHILITE =      MenuItemFlags.MF_UNHILITE,
    MFS_DEFAULT =       MenuItemFlags.MF_DEFAULT,
    MFS_MASK =          0x0000108B,
    MFS_HOTTRACKDRAWN = 0x10000000,
    MFS_CACHEDBMP =     0x20000000,
    MFS_BOTTOMGAPDROP = 0x40000000,
    MFS_TOPGAPDROP =    0x80000000,
    MFS_GAPDROP =       0xC0000000
  }
  #endregion

  #region QueryContextMenuFlags

  /// <summary>
  /// QueryContextMenuFlags
  /// </summary>
  public enum QueryContextMenuFlags: uint
  {
    CMF_NORMAL		= 0x00000000,
    CMF_DEFAULTONLY	= 0x00000001,
    CMF_VERBSONLY	= 0x00000002,
    CMF_EXPLORE		= 0x00000004,
    CMF_NOVERBS		= 0x00000008,
    CMF_CANRENAME	= 0x00000010,
    CMF_NODEFAULT	= 0x00000020,
    CMF_INCLUDESTATIC= 0x00000040,
    CMF_RESERVED	= 0xffff0000      // View specific
  } // enm QueryContextMenuFlags

  #endregion

  #region TrackPopupMenuFlags

  /// <summary>
  /// TrackPopupMenuFlags 
  /// </summary>
  public enum TrackPopupMenuFlags :uint
  {
    TPM_LEFTBUTTON     = 0x0000,
    TPM_RIGHTBUTTON    = 0x0002,
    TPM_LEFTALIGN      = 0x0000,
    TPM_CENTERALIGN    = 0x0004,
    TPM_RIGHTALIGN     = 0x0008,
    TPM_TOPALIGN       = 0x0000,
    TPM_VCENTERALIGN   = 0x0010,
    TPM_BOTTOMALIGN    = 0x0020,
    TPM_HORIZONTAL     = 0x0000,
    TPM_VERTICAL       = 0x0040,
    TPM_NONOTIFY       = 0x0080,     /* Don't send any notification msgs */
    TPM_RETURNCMD      = 0x0100,
    TPM_RECURSE        = 0x0001,
    TPM_HORPOSANIMATION = 0x0400,
    TPM_HORNEGANIMATION = 0x0800,
    TPM_VERPOSANIMATION = 0x1000,
    TPM_VERNEGANIMATION = 0x2000,
    TPM_NOANIMATION     = 0x4000,
    TPM_LAYOUTRTL       = 0x8000,
  } // enum TrackpipupMenuFlags

  #endregion

  #region MenuItemTypes

  /// <summary>
  /// MenuItemTypes 
  /// </summary>
  public enum MenuItemTypes :long
  {
    MFT_STRING          = 0x00000000L,
    MFT_BITMAP          = 0x00000004L,
    MFT_MENUBARBREAK    = 0x00000020L,
    MFT_MENUBREAK       = 0x00000040L,
    MFT_OWNERDRAW       = 0x00000100L,
    MFT_RADIOCHECK      = 0x00000200L,
    MFT_SEPARATOR       = 0x00000800L,
    MFT_RIGHTORDER      = 0x00002000L,
    MFT_RIGHTJUSTIFY    = 0x00004000L,
  } // enum MenuItemTypes

  #endregion

  #region ClipboardFormats

  /// <summary>
  /// ClipboardFormats 
  /// </summary>
  public	enum ClipboardFormats : uint
  {
    CLIPFORMAT_TEXT =		1,
    CLIPFORMAT_BITMAP =		2,
    CLIPFORMAT_METAFILEPICT= 3,
    CLIPFORMAT_SYLK =		4,
    CLIPFORMAT_DIF =		5,
    CLIPFORMAT_TIFF =		6,
    CLIPFORMAT_OEMTEXT =	7,
    CLIPFORMAT_DIB =		8,
    CLIPFORMAT_PALETTE =	9,
    CLIPFORMAT_PENDATA =	10,
    CLIPFORMAT_RIFF =		11,
    CLIPFORMAT_WAVE =		12,
    CLIPFORMAT_UNICODETEXT= 13,
    CLIPFORMAT_ENHMETAFILE= 14,
    CLIPFORMAT_HDROP =		15,
    CLIPFORMAT_LOCALE =		16,
    CLIPFORMAT_MAX =		17,

    CLIPFORMAT_OWNERDISPLAY=0x0080,
    CLIPFORMAT_DSPTEXT =	0x0081,
    CLIPFORMAT_DSPBITMAP =	0x0082,
    CLIPFORMAT_DSPMETAFILEPICT= 0x0083,
    CLIPFORMAT_DSPENHMETAFILE = 0x008E,

    CLIPFORMAT_PRIVATEFIRST=0x0200,
    CLIPFORMAT_PRIVATELAST=	0x02FF,

    CLIPFORMAT_GDIOBJFIRST =0x0300,
    CLIPFORMAT_GDIOBJLAST =	0x03FF
  }

  #endregion

  #region TargetDevices

  /// <summary>
  /// TargetDevices
  /// </summary>
  public	enum TargetDevices: uint
  {
    DVASPECT_CONTENT = 1,
    DVASPECT_THUMBNAIL = 2,
    DVASPECT_ICON = 4,
    DVASPECT_DOCPRINT = 8
  }

  #endregion

  #region StorageMediumTypes

  /// <summary>
  /// StorageMediumTypes 
  /// </summary>
  public	enum StorageMediumTypes: uint
  {
    TYMED_HGLOBAL = 1,
    TYMED_FILE =	2,
    TYMED_ISTREAM = 4,
    TYMED_ISTORAGE= 8,
    TYMED_GDI =		16,
    TYMED_MFPICT =	32,
    TYMED_ENHMF	=	64,
    TYMED_NULL=		0
  }

  #endregion


  #region ShellSpecialFolders
 
  /// <summary>
  /// ShellSpecialFolders / CSIDL
  /// </summary>
  [Flags()]
  public enum ShellSpecialFolders
  {
    CSIDL_DESKTOP                   = 0x0000,		 // <desktop>
    CSIDL_INTERNET                  = 0x0001,
    CSIDL_PROGRAMS                  = 0x0002,        // Start Menu\Programs
    CSIDL_CONTROLS                  = 0x0003,        // My Computer\Control Panel
    CSIDL_PRINTERS                  = 0x0004,        // My Computer\Printers
    CSIDL_PERSONAL                  = 0x0005,        // My Documents
    CSIDL_FAVORITES                 = 0x0006,        // <user name>\Favorites
    CSIDL_STARTUP                   = 0x0007,        // Start Menu\Programs\Startup
    CSIDL_RECENT                    = 0x0008,        // <user name>\Recent
    CSIDL_SENDTO                    = 0x0009,        // <user name>\SendTo
    CSIDL_BITBUCKET                 = 0x000a,        // <desktop>\Recycle Bin
    CSIDL_STARTMENU                 = 0x000b,        // <user name>\Start Menu
    CSIDL_MYDOCUMENTS               = 0x000c,        // logical "My Documents" desktop icon
    CSIDL_MYMUSIC                   = 0x000d,        // "My Music" folder
    CSIDL_MYVIDEO                   = 0x000e,        // "My Videos" folder
    CSIDL_DESKTOPDIRECTORY          = 0x0010,        // <user name>\Desktop
    CSIDL_DRIVES                    = 0x0011,        // My Computer
    CSIDL_NETWORK                   = 0x0012,        // Network Neighborhood (My Network Places)
    CSIDL_NETHOOD                   = 0x0013,        // <user name>\nethood
    CSIDL_FONTS                     = 0x0014,        // windows\fonts
    CSIDL_TEMPLATES                 = 0x0015,
    CSIDL_COMMON_STARTMENU          = 0x0016,        // All Users\Start Menu
    CSIDL_COMMON_PROGRAMS           = 0X0017,        // All Users\Start Menu\Programs
    CSIDL_COMMON_STARTUP            = 0x0018,        // All Users\Startup
    CSIDL_COMMON_DESKTOPDIRECTORY   = 0x0019,        // All Users\Desktop
    CSIDL_APPDATA                   = 0x001a,        // <user name>\Application Data
    CSIDL_PRINTHOOD                 = 0x001b,        // <user name>\PrintHood
    CSIDL_LOCAL_APPDATA             = 0x001c,        // <user name>\Local Settings\Applicaiton Data (non roaming)
    CSIDL_ALTSTARTUP                = 0x001d,        // non localized startup
    CSIDL_COMMON_ALTSTARTUP         = 0x001e,        // non localized common startup
    CSIDL_COMMON_FAVORITES          = 0x001f,
    CSIDL_INTERNET_CACHE            = 0x0020,
    CSIDL_COOKIES                   = 0x0021,
    CSIDL_HISTORY                   = 0x0022,
    CSIDL_COMMON_APPDATA            = 0x0023,        // All Users\Application Data
    CSIDL_WINDOWS                   = 0x0024,        // GetWindowsDirectory()
    CSIDL_SYSTEM                    = 0x0025,        // GetSystemDirectory()
    CSIDL_PROGRAM_FILES             = 0x0026,        // C:\Program Files
    CSIDL_MYPICTURES                = 0x0027,        // C:\Program Files\My Pictures
    CSIDL_PROFILE                   = 0x0028,        // USERPROFILE
    CSIDL_SYSTEMX86                 = 0x0029,        // x86 system directory on RISC
    CSIDL_PROGRAM_FILESX86          = 0x002a,        // x86 C:\Program Files on RISC
    CSIDL_PROGRAM_FILES_COMMON      = 0x002b,        // C:\Program Files\Common
    CSIDL_PROGRAM_FILES_COMMONX86   = 0x002c,        // x86 Program Files\Common on RISC
    CSIDL_COMMON_TEMPLATES          = 0x002d,        // All Users\Templates
    CSIDL_COMMON_DOCUMENTS          = 0x002e,        // All Users\Documents
    CSIDL_COMMON_ADMINTOOLS         = 0x002f,        // All Users\Start Menu\Programs\Administrative Tools
    CSIDL_ADMINTOOLS                = 0x0030,        // <user name>\Start Menu\Programs\Administrative Tools
    CSIDL_CONNECTIONS               = 0x0031,        // Network and Dial-up Connections
    CSIDL_COMMON_MUSIC              = 0x0035,        // All Users\My Music
    CSIDL_COMMON_PICTURES           = 0x0036,        // All Users\My Pictures
    CSIDL_COMMON_VIDEO              = 0x0037,        // All Users\My Video
    CSIDL_RESOURCES                 = 0x0038,        // Resource Direcotry
    CSIDL_RESOURCES_LOCALIZED       = 0x0039,        // Localized Resource Direcotry
    CSIDL_COMMON_OLINKS          = 0x003a,        // Links to All Users OEM specific apps
    CSIDL_CDBURN_AREA               = 0x003b,        // USERPROFILE\Local Settings\Application Data\Microsoft\CD Burning
    COMPUTERSNEARME           = 0x003d,        // Computers Near Me (computered from Workgroup membership)
    CSIDL_FLAG_CREATE               = 0x8000,        // combine with  value to force folder creation in SHGetFolderPath()
    CSIDL_FLAG_DONT_VERIFY          = 0x4000,        // combine with  value to return an unverified folder path
    CSIDL_FLAG_NO_ALIAS             = 0x1000,        // combine with  value to insure non-alias versions of the pidl
    CSIDL_FLAG_PER_USER_INIT        = 0x0800,        // combine with  value to indicate per-user init (eg. upgrade)
    CSIDL_FLAG_MASK                 = 0xFF00,        // mask for all possible flag values
  }

  #endregion

  #region ShellFolderGetaFromIDList

  /// <summary>
  /// ShellFolderGetaFromIDList
  /// </summary>
  [Flags()]
  public enum ShellFolderGetaFromIDList :int
  {
    SHGDFIL_FINDDATA = 1,
    SHGDFIL_NETRESOURCE = 2,
    SHGDFIL_DESCRIPTIONID = 3
  }

  #endregion

  #region ShellFolderEnumObjectsTypes

  /// <summary>
  /// ShellFolderEnumObjectsTypes 
  /// </summary>
  [Flags()]
  public enum ShellFolderEnumObjectsTypes
  {
    SHCONTF_FOLDERS = 0x20,
    SHCONTF_NONFOLDERS = 0x40,
    SHCONTF_INCLUDEHIDDEN = 0x80,
    SHCONTF_INIT_ON_FIRST_NEXT = 0x100,
    SHCONTF_NETPRINTERSRCH = 0x200,
    SHCONTF_SHAREABLE = 0x400,
    SHCONTF_STORAGE = 0x800
  } 

  #endregion

  #region ShellFolderAttributes

  /// <summary>
  /// ShellFolderAttributes 
  /// </summary>
  [Flags()]
  public enum ShellFolderAttributes
  {
    SFGAOF_CANCOPY = 0x1,
    SFGAOF_CANMOVE = 0x2,
    SFGAOF_CANLINK = 0x4,
    SFGAOF_STORAGE = 0x8,
    SFGAOF_CANRENAME = 0x10,
    SFGAOF_CANDELETE = 0x20,
    SFGAOF_HASPROPSHEET = 0x40,
    SFGAOF_DROPTARGET = 0x100,
    SFGAOF_CAPABILITYMASK = 0x177,
    SFGAOF_ENCRYPTED = 0x2000,
    SFGAOF_ISSLOW = 0x4000,
    SFGAOF_GHOSTED = 0x8000,
    SFGAOF_LINK = 0x10000,
    SFGAOF_SHARE = 0x20000,
    SFGAOF_READONLY = 0x40000,
    SFGAOF_HIDDEN = 0x80000,
    SFGAOF_DISPLAYATTRMASK = 0xFC000,
    SFGAOF_FILESYSANCESTOR = 0x10000000,
    SFGAOF_FOLDER = 0x20000000,
    SFGAOF_FILESYSTEM = 0x40000000,
    SFGAOF_HASSUBFOLDER = unchecked( (int) 0x80000000 ),
    SFGAOF_CONTENTSMASK = unchecked( (int) 0x80000000 ),
    SFGAOF_VALIDATE = 0x1000000,
    SFGAOF_REMOVABLE = 0x2000000,
    SFGAOF_COMPRESSED = 0x4000000,
    SFGAOF_BROWSABLE = 0x8000000,
    SFGAOF_NONENUMERATED = 0x100000,
    SFGAOF_NEWCONTENT = 0x200000,
    SFGAOF_CANMONIKER = 0x400000,
    SFGAOF_HASSTORAGE = 0x400000,
    SFGAOF_STREAM = 0x400000,
    SFGAOF_STORAGEANCESTOR = 0x800000,
    SFGAOF_STORAGECAPMASK = 0x70C50008
  }
  
  #endregion

  #region ShellFolderDisplayNames

  /// <summary>
  /// ShellFolderDisplayNames
  /// </summary>
  [Flags()]
  public enum ShellFolderDisplayNames
  {  
    SHGNO_NORMAL = 0x0,
    SHGNO_INFOLDER = 0x1,
    SHGNO_FOREDITING = 0x1000,
    SHGNO_FORADDRESSBAR = 0x4000, 
    SHGNO_FORPARSING = 0x8000,
  } // enum enum ShellFolderDisplayNames

  #endregion

  #region ShellExecuteFlags

  /// <summary>
  /// Shell Execute Flags 
  /// </summary>
  public enum ShellExecuteFlags
  {
    SEE_CLASSNAME        = 0x00000001,
    SEE_CLASSKEY         = 0x00000003,
    SEE_IDLIST           = 0x00000004,
    SEE_INVOKEIDLIST     = 0x0000000c,
    SEE_ICON             = 0x00000010,
    SEE_HOTKEY           = 0x00000020,
    SEE_NOCLOSEPROCESS   = 0x00000040,
    SEE_CONNECTNETDRV    = 0x00000080,
    SEE_FLAG_DDEWAIT     = 0x00000100,
    SEE_DOENVSUBST       = 0x00000200,
    SEE_FLAG_NO_UI       = 0x00000400,
    SEE_UNICODE          = 0x00004000,
    SEE_NO_CONSOLE       = 0x00008000,
    SEE_ASYNCOK          = 0x00100000,
    SEE_HMONITOR         = 0x00200000,
    SEE_NOQUERYCLASSSTORE= 0x01000000,
    SEE_WAITFORINPUTIDLE = 0x02000000,
    SEE_FLAG_LOG_USAGE   = 0x04000000,
  } // enum ShellExecuteFlags

  #endregion

  #region ShellGetFileInformationFlags

  /// <summary>
  /// Shell Get File Information Flags 
  /// </summary>
  public enum ShellGetFileInformationFlags
  {
    SHGFI_SMALLICON       = 0x00000001,
    SHGFI_OPENICON          = 0x00000002,
    SHGFI_LARGEICON         = 0x00000000,
    SHGFI_ICON              = 0x00000100,
    SHGFI_DISPLAYNAME       = 0x00000200,
    SHGFI_TYPENAME          = 0x00000400,
    SHGFI_SYSICONINDEX      = 0x00004000,
    SHGFI_LINKOVERLAY       = 0x00008000,
    SHGFI_USEFILEATTRIBUTES = 0x00000010
  } // enum ShellGetFileInformationFlags

  #endregion

  #region FormHitTest

  /// <summary>
  /// Form HistTest
  /// </summary>
  public enum FormHitTest : int
  {
    MinButton = 8,
    MaxButton = 9,
    CloseButton = 20
  } // enum FormHitTest

  #endregion

  #region SystemCommands

  /// <summary>
  /// System Commands 
  /// </summary>
  public enum SystemCommands
  {
    /// <summary>
    /// Sizes the window
    /// </summary>
    SC_SIZE         = 0xF000,
    /// <summary>
    /// Moves the window
    /// </summary>
    SC_MOVE         = 0xF010,
    /// <summary>
    /// Minimizes the window
    /// </summary>
    SC_MINIMIZE     = 0xF020,
    /// <summary>
    /// Maximizes the window
    /// </summary>
    SC_MAXIMIZE     = 0xF030,
    /// <summary>
    /// Moves to the next window
    /// </summary>
    SC_NEXTWINDOW   = 0xF040,
    /// <summary>
    /// Moves to the previous window
    /// </summary>
    SC_PREVWINDOW   = 0xF050,
    /// <summary>
    /// Closes the window
    /// </summary>
    SC_CLOSE        = 0xF060,
    /// <summary>
    /// Scrolls vertically
    /// </summary>
    SC_VSCROLL      = 0xF070,
    /// <summary>
    /// Scrolls horizontally
    /// </summary>
    SC_HSCROLL      = 0xF080,
    /// <summary>
    /// Retrieves the window menu as a result of a mouse click
    /// </summary>
    SC_MOUSEMENU    = 0xF090,
    /// <summary>
    /// Retrieves the window menu as a result of a keystroke
    /// </summary>
    SC_KEYMENU      = 0xF100,
    SC_ARRANGE      = 0xF110,
    /// <summary>
    /// Restores the window to its normal position and size
    /// </summary>
    SC_RESTORE      = 0xF120,
    /// <summary>
    /// Activates the Start menu
    /// </summary>
    SC_TASKLIST     = 0xF130,
    /// <summary>
    /// Executes the screen saver application specified in the [boot] section of the System.ini file
    /// </summary>
    SC_SCREENSAVE   = 0xF140,
    /// <summary>
    /// Activates the window associated with the application-specified hot key. The lParam parameter identifies the window to activate
    /// </summary>
    SC_HOTKEY       = 0xF150,
    /// <summary>
    /// Selects the default item; the user double-clicked the window menu
    /// </summary>
    SC_DEFAULT      = 0xF160,
    /// <summary>
    /// Sets the state of the display. This command supports devices that have power-saving features, such as a battery-powered personal computer. 
    /// The lParam parameter can have the following values:
    ///     1 - the display is going to low power
    ///     2 - the display is being shut off
    /// </summary>
    SC_MONITORPOWER = 0xF170,
    /// <summary>
    /// Changes the cursor to a question mark with a pointer. If the user then clicks a control in the dialog box, the control receives a WM_HELP message
    /// </summary>
    SC_CONTEXTHELP  = 0xF180,
    SC_SEPARATOR    = 0xF00F,
    SC_ICON         = SC_MINIMIZE,
    SC_ZOOM         = SC_MAXIMIZE
  } // enum SystemCommands

  #endregion

  #region GetCommandStringInformations

  /// <summary>
  /// GetCommandString informations
  /// </summary>
  public enum GetCommandStringInformations
  {
    GCS_VERB        = 0x00000004,
    GCS_HELPTEXT    = 0x00000005,
    GCS_VALIDATE    = 0x00000006,
  }

  #endregion

  #region FileOperations

  /// <summary>
  /// File Operations 
  /// </summary>
  public enum FileOperations: int
  {
    FO_MOVE           = 0x0001,
    FO_COPY           = 0x0002,
    FO_DELETE         = 0x0003,
    FO_RENAME         = 0x0004,
  }

  #endregion

  #region FileOperation Flags 

  /// <summary>
  /// FileOperation Flag 
  /// </summary>
  public enum FileOperationFlags: short
  {
    FOF_MULTIDESTFILES         = 0x0001,
    FOF_CONFIRMMOUSE           = 0x0002,
    /// <summary>
    /// Don't create progress/report
    /// </summary>
    FOF_SILENT                 = 0x0004,
    FOF_RENAMEONCOLLISION      = 0x0008,
    /// <summary>
    /// Don't prompt the user.
    /// </summary>
    FOF_NOCONFIRMATION         = 0x0010,
    /// <summary>
    /// Fill in SHFILEOPSTRUCT.hNameMappings
    /// </summary>
    FOF_WANTMAPPINGHANDLE      = 0x0020,
    FOF_ALLOWUNDO              = 0x0040,
    /// <summary>
    /// On *.*, do only files
    /// </summary>
    FOF_FILESONLY              = 0x0080,
  }

  #endregion

  #region ListViewMessages

  /// <summary>
  /// ListView Messages 
  /// </summary>
  public enum ListViewMessages : int
  {
    LVM_FIRST				= 0x1000,
    LVM_SCROLL				= LVM_FIRST + 20,
    LVM_GETITEM				= LVM_FIRST + 75,
    LVM_SETITEM				= LVM_FIRST + 76,
    LVM_GETITEMTEXTW		= LVM_FIRST + 115,
    LVM_SETCOLUMNWIDTH		= LVM_FIRST + 30,
    LVM_LVSCW_AUTOSIZE			= -1,
    LVM_LVSCW_AUTOSIZE_USEHEADER= -2,
    LVM_SETITEMSTATE		= LVM_FIRST + 43,
    LVM_INSERTITEMA			= LVM_FIRST + 77,
    LVM_DELETEITEM			= LVM_FIRST + 8,
    LVM_GETITEMCOUNT		= LVM_FIRST + 4,
    LVM_GETCOUNTPERPAGE		= LVM_FIRST + 40,
    LVM_GETSUBITEMRECT		= LVM_FIRST + 56,
    LVM_SUBITEMHITTEST		= LVM_FIRST + 57,
    LVM_GETCOLUMN			= LVM_FIRST + 25,
    LVM_SETCOLUMN			= LVM_FIRST + 26,
    LVM_GETCOLUMNORDERARRAY	= LVM_FIRST + 59,
    LVM_SETCOLUMNORDERARRAY	= LVM_FIRST + 58,
    LVM_SETEXTENDEDLISTVIEWSTYLE= LVM_FIRST + 54,
    LVM_GETEXTENDEDLISTVIEWSTYLE= LVM_FIRST + 55,
    LVM_EDITLABELW			= LVM_FIRST + 118,
    LVM_GETITEMRECT        = LVM_FIRST + 14,
    LVM_HITTEST            = LVM_FIRST + 18,
    LVM_GETEDITCONTROL     = LVM_FIRST + 24,
    LVM_CANCELEDITLABEL	   = LVM_FIRST + 179,
    LVM_GETHEADER          = LVM_FIRST + 31,
    LVM_REDRAWITEMS        = LVM_FIRST + 21,
    LVM_GETSELECTIONMARK   = LVM_FIRST + 66,
    LVM_SETSELECTIONMARK   = LVM_FIRST + 67,
    LVM_ENSUREVISIBLE       = (LVM_FIRST + 19),
  }

  #endregion

  #region ListViewNotifications

  /// <summary>
  /// ListView Notifications
  /// </summary>
  public enum ListViewNotifications
  {
    LVN_FIRST               = (0-100),
    LVN_LAST                = (0-199),
    LVN_ITEMCHANGING        = (LVN_FIRST-0),
    LVN_ITEMCHANGED         = (LVN_FIRST-1),
    LVN_INSERTITEM          = (LVN_FIRST-2),
    LVN_DELETEITEM          = (LVN_FIRST-3),
    LVN_DELETEALLITEMS      = (LVN_FIRST-4),
    LVN_BEGINLABELEDITW     = (LVN_FIRST-75),
    LVN_ENDLABELEDITW       = (LVN_FIRST-76),
    LVN_COLUMNCLICK         = (LVN_FIRST-8),
    LVN_BEGINDRAG           = (LVN_FIRST-9),
    LVN_BEGINRDRAG          = (LVN_FIRST-11),
    LVN_ODCACHEHINT         = (LVN_FIRST-13),
    LVN_ODFINDITEMA         = (LVN_FIRST-52),
    LVN_ODFINDITEMW         = (LVN_FIRST-79),
    LVN_ITEMACTIVATE        = (LVN_FIRST-14),
    LVN_ODSTATECHANGED      = (LVN_FIRST-15),
    LVN_ODFINDITEM          = LVN_ODFINDITEMW,
    LVN_HOTTRACK            = (LVN_FIRST-21),
    LVN_GETDISPINFOA        = (LVN_FIRST-50),
    LVN_GETDISPINFOW        = (LVN_FIRST-77),
    LVN_SETDISPINFOA        = (LVN_FIRST-51),
    LVN_MARQUEEBEGIN        = (LVN_FIRST-56),
    LVN_SETDISPINFOW        = (LVN_FIRST-78),
    LVN_BEGINLABELEDIT      = LVN_BEGINLABELEDITW,
    LVN_ENDLABELEDIT        = LVN_ENDLABELEDITW,
    LVN_GETDISPINFO         = LVN_GETDISPINFOW,
    LVN_SETDISPINFO         = LVN_SETDISPINFOW,
    LVN_BEGINSCROLL         = (LVN_FIRST-80),
    LVN_ENDSCROLL           = (LVN_FIRST-81),
  }

  #endregion

  #region ListViewItemFlags

  /// <summary>
  /// ListViewItem Flags
  /// </summary>
  public enum ListViewItemFlags : int
  {
    LVIF_TEXT               = 0x0001,
    LVIF_IMAGE              = 0x0002,
    LVIF_PARAM              = 0x0004,
    LVIF_STATE              = 0x0008,
    LVIF_INDENT             = 0x0010,
    LVIF_NORECOMPUTE        = 0x0800,
    LVIF_GROUPID            = 0x0100,
    LVIF_COLUMNS            = 0x0200,
  }

  #endregion

  #region ListViewItemStates

  /// <summary>
  /// ListViewItemState 
  /// </summary>
  public enum ListViewItemStates : int
  {
    LVIS_FOCUSED            = 0x0001,
    LVIS_SELECTED           = 0x0002,
    LVIS_CUT                = 0x0004,
    LVIS_DROPHILITED        = 0x0008,
    LVIS_GLOW               = 0x0010,
    LVIS_ACTIVATING         = 0x0020,
    LVIS_OVERLAYMASK        = 0x0F00,
    LVIS_STATEIMAGEMASK     = 0xF000,
  }

  #endregion

  #region ListViewExtendedStyles 

  /// <summary>
  /// List View Extended Styles
  /// </summary>
  public enum ListViewExtendedStyles
  {
    LVS_EX_GRIDLINES        =0x00000001,
    LVS_EX_SUBITEMIMAGES    =0x00000002,
    LVS_EX_CHECKBOXES       =0x00000004,
    LVS_EX_TRACKSELECT      =0x00000008,
    LVS_EX_HEADERDRAGDROP   =0x00000010,
    LVS_EX_FULLROWSELECT    =0x00000020, 
    LVS_EX_ONECLICKACTIVATE =0x00000040,
    LVS_EX_TWOCLICKACTIVATE =0x00000080,
    LVS_EX_FLATSB           =0x00000100,
    LVS_EX_REGIONAL         =0x00000200,
    LVS_EX_INFOTIP          =0x00000400,
    LVS_EX_UNDERLINEHOT     =0x00000800,
    LVS_EX_UNDERLINECOLD    =0x00001000,
    LVS_EX_MULTIWORKAREAS   =0x00002000,
    LVS_EX_LABELTIP         =0x00004000,
    LVS_EX_BORDERSELECT     =0x00008000, 
    LVS_EX_DOUBLEBUFFER     =0x00010000,
    LVS_EX_HIDELABELS       =0x00020000,
    LVS_EX_SINGLEROW        =0x00040000,
    LVS_EX_SNAPTOGRID       =0x00080000,  
    LVS_EX_SIMPLESELECT     =0x00100000  
  }

  #endregion

  #region ListViewSubItemPortion

  /// <summary>
  /// List View sub item portion
  /// </summary>
  public enum ListViewSubItemPortion
  {
    LVIR_BOUNDS = 0,
    LVIR_ICON   = 1,
    LVIR_LABEL  = 2
  }

  #endregion

  #region ListViewHitTestFlags

  /// <summary>
  /// ListView HitTest Flags 
  /// </summary>
  public enum ListViewHitTestFlags
  {
    LVHT_NOWHERE           = 0x0001,
    LVHT_ONITEMICON        = 0x0002,
    LVHT_ONITEMLABEL       = 0x0004,
    LVHT_ONITEMSTATEICON   = 0x0008,
    LVHT_ONITEM            = LVHT_ONITEMICON | LVHT_ONITEMLABEL | LVHT_ONITEMSTATEICON,
    LVHT_ABOVE             = 0x0008,
    LVHT_BELOW             = 0x0010,
    LVHT_TORIGHT           = 0x0020,
    LVHT_TOLEFT            = 0x0040,
  }

  #endregion

  #region ReflectedMessages

  /// <summary>
  /// Reflected Messages
  /// </summary>
  public enum ReflectedMessages
  {
    OCM__BASE               = (WindowMessages.WM_USER + 0x1c00),
    OCM_COMMAND             = (OCM__BASE + WindowMessages.WM_COMMAND),
    OCM_CTLCOLORBTN         = (OCM__BASE + WindowMessages.WM_CTLCOLORBTN),
    OCM_CTLCOLOREDIT        = (OCM__BASE + WindowMessages.WM_CTLCOLOREDIT),
    OCM_CTLCOLORDLG         = (OCM__BASE + WindowMessages.WM_CTLCOLORDLG),
    OCM_CTLCOLORLISTBOX     = (OCM__BASE + WindowMessages.WM_CTLCOLORLISTBOX),
    OCM_CTLCOLORMSGBOX      = (OCM__BASE + WindowMessages.WM_CTLCOLORMSGBOX),
    OCM_CTLCOLORSCROLLBAR   = (OCM__BASE + WindowMessages.WM_CTLCOLORSCROLLBAR),
    OCM_CTLCOLORSTATIC      = (OCM__BASE + WindowMessages.WM_CTLCOLORSTATIC),
    OCM_CTLCOLOR            = (OCM__BASE + WindowMessages.WM_CTLCOLOR),
    OCM_DRAWITEM            = (OCM__BASE + WindowMessages.WM_DRAWITEM),
    OCM_MEASUREITEM         = (OCM__BASE + WindowMessages.WM_MEASUREITEM),
    OCM_DELETEITEM          = (OCM__BASE + WindowMessages.WM_DELETEITEM),
    OCM_VKEYTOITEM          = (OCM__BASE + WindowMessages.WM_VKEYTOITEM),
    OCM_CHARTOITEM          = (OCM__BASE + WindowMessages.WM_CHARTOITEM),
    OCM_COMPAREITEM         = (OCM__BASE + WindowMessages.WM_COMPAREITEM),
    OCM_HSCROLL             = (OCM__BASE + WindowMessages.WM_HSCROLL),
    OCM_VSCROLL             = (OCM__BASE + WindowMessages.WM_VSCROLL),
    OCM_PARENTNOTIFY        = (OCM__BASE + WindowMessages.WM_PARENTNOTIFY),
    OCM_NOTIFY              = (OCM__BASE + WindowMessages.WM_NOTIFY),
  }

  #endregion

  #region HeaderItemFlags

  /// <summary>
  /// HeaderItem flags
  /// </summary>
  public enum HeaderItemFlags
  {
    HDI_WIDTH               = 0x0001,
    HDI_HEIGHT              = HDI_WIDTH,
    HDI_TEXT                = 0x0002,
    HDI_FORMAT              = 0x0004,
    HDI_LPARAM              = 0x0008,
    HDI_BITMAP              = 0x0010,
    HDI_IMAGE               = 0x0020,
    HDI_DI_SETITEM          = 0x0040,
    HDI_ORDER               = 0x0080
  }

  #endregion

  #region HeaderControlMessages

  /// <summary>
  /// Header Control Messages
  /// </summary>
  public enum HeaderControlMessages : int
  {
    HDM_FIRST        =  0x1200,
    HDM_GETITEMRECT  = (HDM_FIRST + 7),
    HDM_HITTEST      = (HDM_FIRST + 6),
    HDM_SETIMAGELIST = (HDM_FIRST + 8),
    HDM_GETITEMW     = (HDM_FIRST + 11),
    HDM_ORDERTOINDEX = (HDM_FIRST + 15),
    HDM_SETITEM      = (HDM_FIRST + 12),
    HDM_SETORDERARRAY= (HDM_FIRST + 18),
  }

  #endregion

  #region HeaderControlNotifications

  /// <summary>
  /// Header Control Notifications
  /// </summary>
  public enum HeaderControlNotifications
  {
    HDN_FIRST              = (0-300),
    HDN_ITEMCHANGING       = (HDN_FIRST-20),
    HDN_ITEMCHANGED        = (HDN_FIRST-21),
    HDN_ITEMCLICK          = (HDN_FIRST-22),
    HDN_ITEMDBLCLICK       = (HDN_FIRST-23),
    HDN_DIVIDERDBLCLICK    = (HDN_FIRST-25),
    HDN_BEGINTRACK         = (HDN_FIRST-26),
    HDN_ENDTRACK           = (HDN_FIRST-27),
    HDN_TRACK              = (HDN_FIRST-28),
    HDN_GETDISPINFO        = (HDN_FIRST-29),
    HDN_BEGINDRAG          = (HDN_FIRST-10),
    HDN_ENDDRAG            = (HDN_FIRST-11),
    HDN_FILTERCHANGE       = (HDN_FIRST-12),
    HDN_FILTERBTNCLICK     = (HDN_FIRST-13),
  }

  #endregion

  #region HeaderControlHitTestFlags

  /// <summary>
  /// Header Control HitTest Flags
  /// </summary>
  public enum HeaderControlHitTestFlags : uint
  {
    HHT_NOWHERE             = 0x0001,
    HHT_ONHEADER            = 0x0002,
    HHT_ONDIVIDER           = 0x0004,
    HHT_ONDIVOPEN           = 0x0008,
    HHT_ABOVE               = 0x0100,
    HHT_BELOW               = 0x0200,
    HHT_TORIGHT             = 0x0400,
    HHT_TOLEFT              = 0x0800
  } // Enum HeaderControlHitTestFlags

  #endregion

  #region CustomDrawReturnFlags

  /// <summary>
  /// Custom Draw Return Flags 
  /// </summary>
  public enum CustomDrawReturnFlags
  {
    CDRF_DODEFAULT          = 0x00000000,
    CDRF_NEWFONT            = 0x00000002,
    CDRF_SKIPDEFAULT        = 0x00000004,
    CDRF_NOTIFYPOSTPAINT    = 0x00000010,
    CDRF_NOTIFYITEMDRAW     = 0x00000020,
    CDRF_NOTIFYSUBITEMDRAW  = 0x00000020, 
    CDRF_NOTIFYPOSTERASE    = 0x00000040
  } // enum CustomDrawReturnFlags

  #endregion

  #region CustomDrawItemStateFlags

  /// <summary>
  /// CustomDrawItemStateFlags
  /// </summary>
  public enum CustomDrawItemStateFlags : uint 
  {
    CDIS_SELECTED       = 0x0001,
    CDIS_GRAYED         = 0x0002,
    CDIS_DISABLED       = 0x0004,
    CDIS_CHECKED        = 0x0008,
    CDIS_FOCUS          = 0x0010,
    CDIS_DEFAULT        = 0x0020,
    CDIS_HOT            = 0x0040,
    CDIS_MARKED         = 0x0080,
    CDIS_INDETERMINATE  = 0x0100
  } // enum CustomDrawItemStateFlags

  #endregion

  #region CustomDrawDrawStateFlags

  /// <summary>
  /// CustomDrawDrawStateFlags
  /// </summary>
  public enum CustomDrawDrawStateFlags
  {
    CDDS_PREPAINT           = 0x00000001,
    CDDS_POSTPAINT          = 0x00000002,
    CDDS_PREERASE           = 0x00000003,
    CDDS_POSTERASE          = 0x00000004,
    CDDS_ITEM               = 0x00010000,
    CDDS_ITEMPREPAINT       = (CDDS_ITEM | CDDS_PREPAINT),
    CDDS_ITEMPOSTPAINT      = (CDDS_ITEM | CDDS_POSTPAINT),
    CDDS_ITEMPREERASE       = (CDDS_ITEM | CDDS_PREERASE),
    CDDS_ITEMPOSTERASE      = (CDDS_ITEM | CDDS_POSTERASE),
    CDDS_SUBITEM            = 0x00020000
  } // enum CustomDrawDrawStateFlags

  #endregion

  #region PatBltTypes

  /// <summary>
  /// PatBlt Types
  /// </summary>
  public enum PatBltTypes
  {
    SRCCOPY          =   0x00CC0020,
    SRCPAINT         =   0x00EE0086,
    SRCAND           =   0x008800C6,
    SRCINVERT        =   0x00660046,
    SRCERASE         =   0x00440328,
    NOTSRCCOPY       =   0x00330008,
    NOTSRCERASE      =   0x001100A6,
    MERGECOPY        =   0x00C000CA,
    MERGEPAINT       =   0x00BB0226,
    PATCOPY          =   0x00F00021,
    PATPAINT         =   0x00FB0A09,
    PATINVERT        =   0x005A0049,
    DSTINVERT        =   0x00550009,
    BLACKNESS        =   0x00000042,
    WHITENESS        =   0x00FF0062
  } // enum PatBltTypes

  #endregion

  #region BackgroundMode

  /// <summary>
  /// Background Mode
  /// </summary>
  public enum BackgroundMode
  {
    TRANSPARENT = 1,
    OPAQUE = 2
  } // enum BackgroundModes

  #endregion

  #region StrechModeFlags

  /// <summary>
  /// Strech Mode Flags
  /// </summary>
  public enum StrechModeFlags
  {
    BLACKONWHITE		= 1,
    WHITEONBLACK        = 2,
    COLORONCOLOR        = 3,
    HALFTONE            = 4,
    MAXSTRETCHBLTMODE   = 4
  }// enum StrechModeFlags

  #endregion

  #region WindowsHookCodes

  /// <summary>
  /// Windows Hook Codes 
  /// </summary>
  public enum WindowsHookCodes
  {
    WH_MSGFILTER        = (-1),
    WH_JOURNALRECORD    = 0,
    WH_JOURNALPLAYBACK  = 1,
    WH_KEYBOARD         = 2,
    WH_GETMESSAGE       = 3,
    WH_CALLWNDPROC      = 4,
    WH_CBT              = 5,
    WH_SYSMSGFILTER     = 6,
    WH_MOUSE            = 7,
    WH_HARDWARE         = 8,
    WH_DEBUG            = 9,
    WH_SHELL            = 10,
    WH_FOREGROUNDIDLE   = 11,
    WH_CALLWNDPROCRET   = 12,
    WH_KEYBOARD_LL      = 13,
    WH_MOUSE_LL         = 14
  } // enum WindowHookCodes

  #endregion

  #region CursorTypes

  /// <summary>
  /// Cursor Types 
  /// </summary>
  public enum CursorTypes : uint
  {
    IDC_ARROW		= 32512U,
    IDC_IBEAM       = 32513U,
    IDC_WAIT        = 32514U,
    IDC_CROSS       = 32515U,
    IDC_UPARROW     = 32516U,
    IDC_SIZE        = 32640U,
    IDC_ICON        = 32641U,
    IDC_SIZENWSE    = 32642U,
    IDC_SIZENESW    = 32643U,
    IDC_SIZEWE      = 32644U,
    IDC_SIZENS      = 32645U,
    IDC_SIZEALL     = 32646U,
    IDC_NO          = 32648U,
    IDC_HAND        = 32649U,
    IDC_APPSTARTING = 32650U,
    IDC_HELP        = 32651U
  } // enum CursorTypes

  #endregion

  #region SystemMetricsCodes

  /// <summary>
  /// System Metrics Codes
  /// </summary>
  public enum SystemMetricsCodes
  {
    SM_CXSCREEN             = 0,
    SM_CYSCREEN             = 1,
    SM_CXVSCROLL            = 2,
    SM_CYHSCROLL            = 3,
    SM_CYCAPTION            = 4,
    SM_CXBORDER             = 5,
    SM_CYBORDER             = 6,
    SM_CXDLGFRAME           = 7,
    SM_CYDLGFRAME           = 8,
    SM_CYVTHUMB             = 9,
    SM_CXHTHUMB             = 10,
    SM_CXICON               = 11,
    SM_CYICON               = 12,
    SM_CXCURSOR             = 13,
    SM_CYCURSOR             = 14,
    SM_CYMENU               = 15,
    SM_CXFULLSCREEN         = 16,
    SM_CYFULLSCREEN         = 17,
    SM_CYKANJIWINDOW        = 18,
    SM_MOUSEPRESENT         = 19,
    SM_CYVSCROLL            = 20,
    SM_CXHSCROLL            = 21,
    SM_DEBUG                = 22,
    SM_SWAPBUTTON           = 23,
    SM_RESERVED1            = 24,
    SM_RESERVED2            = 25,
    SM_RESERVED3            = 26,
    SM_RESERVED4            = 27,
    SM_CXMIN                = 28,
    SM_CYMIN                = 29,
    SM_CXSIZE               = 30,
    SM_CYSIZE               = 31,
    SM_CXFRAME              = 32,
    SM_CYFRAME              = 33,
    SM_CXMINTRACK           = 34,
    SM_CYMINTRACK           = 35,
    SM_CXDOUBLECLK          = 36,
    SM_CYDOUBLECLK          = 37,
    SM_CXICONSPACING        = 38,
    SM_CYICONSPACING        = 39,
    SM_MENUDROPALIGNMENT    = 40,
    SM_PENWINDOWS           = 41,
    SM_DBCSENABLED          = 42,
    SM_CMOUSEBUTTONS        = 43,
    SM_CXFIXEDFRAME         = SM_CXDLGFRAME, 
    SM_CYFIXEDFRAME         = SM_CYDLGFRAME,  
    SM_CXSIZEFRAME          = SM_CXFRAME,    
    SM_CYSIZEFRAME          = SM_CYFRAME,    
    SM_SECURE               = 44,
    SM_CXEDGE               = 45,
    SM_CYEDGE               = 46,
    SM_CXMINSPACING         = 47,
    SM_CYMINSPACING         = 48,
    SM_CXSMICON             = 49,
    SM_CYSMICON             = 50,
    SM_CYSMCAPTION          = 51,
    SM_CXSMSIZE             = 52,
    SM_CYSMSIZE             = 53,
    SM_CXMENUSIZE           = 54,
    SM_CYMENUSIZE           = 55,
    SM_ARRANGE              = 56,
    SM_CXMINIMIZED          = 57,
    SM_CYMINIMIZED          = 58,
    SM_CXMAXTRACK           = 59,
    SM_CYMAXTRACK           = 60,
    SM_CXMAXIMIZED          = 61,
    SM_CYMAXIMIZED          = 62,
    SM_NETWORK              = 63,
    SM_CLEANBOOT            = 67,
    SM_CXDRAG               = 68,
    SM_CYDRAG               = 69,
    SM_SHOWSOUNDS           = 70,
    SM_CXMENUCHECK          = 71,  
    SM_CYMENUCHECK          = 72,
    SM_SLOWMACHINE          = 73,
    SM_MIDEASTENABLED       = 74,
    SM_MOUSEWHEELPRESENT    = 75,
    SM_XVIRTUALSCREEN       = 76,
    SM_YVIRTUALSCREEN       = 77,
    SM_CXVIRTUALSCREEN      = 78,
    SM_CYVIRTUALSCREEN      = 79,
    SM_CMONITORS            = 80,
    SM_SAMEDISPLAYFORMAT    = 81,
    SM_CMETRICS             = 83
  } // enum SystemMetrics

  #endregion

  #region SystemObjects

  /// <summary>
  /// Windows System Objects
  /// </summary>
  public enum SystemObjects : uint
  {
    // Reserved IDs for system objects
    OBJID_WINDOW        = 0x00000000,
    OBJID_SYSMENU       = 0xFFFFFFFF,
    OBJID_TITLEBAR      = 0xFFFFFFFE,
    OBJID_MENU          = 0xFFFFFFFD,
    OBJID_CLIENT        = 0xFFFFFFFC,
    OBJID_VSCROLL       = 0xFFFFFFFB,
    OBJID_HSCROLL       = 0xFFFFFFFA,
    OBJID_SIZEGRIP      = 0xFFFFFFF9,
    OBJID_CARET         = 0xFFFFFFF8,
    OBJID_CURSOR        = 0xFFFFFFF7,
    OBJID_ALERT         = 0xFFFFFFF6,
    OBJID_SOUND         = 0xFFFFFFF5
  } // enum SystemObjects

  #endregion

  #region TrackerEventFlags

  /// <summary>
  /// Tracker Event Flags
  /// </summary>
  public enum TrackerEventFlags : uint
  {
    TME_HOVER	= 0x00000001,
    TME_LEAVE	= 0x00000002,
    TME_QUERY	= 0x40000000,
    TME_CANCEL	= 0x80000000
  } // enum TrackerEventFlags

  #endregion

  #region DrawTextFormatFlags 

  /// <summary>
  /// Draw Text format flags
  /// </summary>
  public enum DrawTextFormatFlags
  {
    DT_TOP              = 0x00000000,
    DT_LEFT             = 0x00000000,
    DT_CENTER           = 0x00000001,
    DT_RIGHT            = 0x00000002,
    DT_VCENTER          = 0x00000004,
    DT_BOTTOM           = 0x00000008,
    DT_WORDBREAK        = 0x00000010,
    DT_SINGLELINE       = 0x00000020,
    DT_EXPANDTABS       = 0x00000040,
    DT_TABSTOP          = 0x00000080,
    DT_NOCLIP           = 0x00000100,
    DT_EXTERNALLEADING  = 0x00000200,
    DT_CALCRECT         = 0x00000400,
    DT_NOPREFIX         = 0x00000800,
    DT_INTERNAL         = 0x00001000,
    DT_EDITCONTROL      = 0x00002000,
    DT_PATH_ELLIPSIS    = 0x00004000,
    DT_END_ELLIPSIS     = 0x00008000,
    DT_MODIFYSTRING     = 0x00010000,
    DT_RTLREADING       = 0x00020000,
    DT_WORD_ELLIPSIS    = 0x00040000
  } // enum DrawTextFormatFlags

  #endregion

  #region UpdateLayeredWindowFlags

  /// <summary>
  /// Update Layered Windows Flags
  /// </summary>
  public enum UpdateLayeredWindowFlags
  {
    ULW_COLORKEY = 0x00000001,
    ULW_ALPHA    = 0x00000002,
    ULW_OPAQUE   = 0x00000004
  } // enum UpdateLayeredWindowFlags


  #endregion
 
  #region NotificationMessages

  /// <summary>
  /// Notification Messages
  /// </summary>
  public enum NotificationMessages
  {
    NM_FIRST      = (0-0),
    NM_CUSTOMDRAW = (NM_FIRST-12),
    NM_NCHITTEST  = (NM_FIRST-14) 
  } // enum NotificationMessages

  #endregion

  #region WindowsSystemColors

  /// <summary>
  /// System Colors 
  /// </summary>
  public enum WindowsSystemColors : int
  {
    COLOR_SCROLLBAR         = 0,
    COLOR_BACKGROUND        = 1,
    COLOR_ACTIVECAPTION     = 2,
    COLOR_INACTIVECAPTION   = 3,
    COLOR_MENU              = 4,
    COLOR_WINDOW            = 5,
    COLOR_WINDOWFRAME       = 6,
    COLOR_MENUTEXT          = 7,
    COLOR_WINDOWTEXT        = 8,
    COLOR_CAPTIONTEXT       = 9,
    COLOR_ACTIVEBORDER      = 10,
    COLOR_INACTIVEBORDER    = 11,
    COLOR_APPWORKSPACE      = 12,
    COLOR_HIGHLIGHT         = 13,
    COLOR_HIGHLIGHTTEXT     = 14,
    COLOR_BTNFACE           = 15,
    COLOR_BTNSHADOW         = 16,
    COLOR_GRAYTEXT          = 17,
    COLOR_BTNTEXT           = 18,
    COLOR_INACTIVECAPTIONTEXT = 19,
    COLOR_BTNHIGHLIGHT      = 20,
  } // enum WindowsSystemColors

  #endregion

  #region DrawFrameControlFlags

  /// <summary>
  /// Draw Frame Control Flags
  /// </summary>
  public enum DrawFrameControlFlags : uint
  {
    DFC_CAPTION             = 1,
    DFC_MENU                = 2,
    DFC_SCROLL              = 3,
    DFC_BUTTON              = 4,
    DFC_POPUPMENU           = 5,
  } /// enum DrawFrameControlFlags

  #endregion

  #region DrawFrameControlStateFlags 

  /// <summary>
  /// Draw Frame Control State Flags 
  /// </summary>
  public enum DrawFrameControlStateFlags : uint
  {
    DFC_CAPTIONCLOSE       = 0x0000,
    DFC_CAPTIONMIN         = 0x0001,
    DFC_CAPTIONMAX         = 0x0002,
    DFC_CAPTIONRESTORE     = 0x0003,
    DFC_CAPTIONHELP        = 0x0004,

    DFC_MENUARROW          = 0x0000,
    DFC_MENUCHECK          = 0x0001,
    DFC_MENUBULLET         = 0x0002,
    DFC_MENUARROWRIGHT     = 0x0004,
    DFC_SCROLLUP           = 0x0000,
    DFC_SCROLLDOWN         = 0x0001,
    DFC_SCROLLLEFT         = 0x0002,
    DFC_SCROLLRIGHT        = 0x0003,
    DFC_SCROLLCOMBOBOX     = 0x0005,
    DFC_SCROLLSIZEGRIP     = 0x0008,
    DFC_SCROLLSIZEGRIPRIGHT = 0x0010,

    DFC_BUTTONCHECK        = 0x0000,
    DFC_BUTTONRADIOIMAGE   = 0x0001,
    DFC_BUTTONRADIOMASK    = 0x0002,
    DFC_BUTTONRADIO        = 0x0004,
    DFC_BUTTON3STATE       = 0x0008,
    DFC_BUTTONPUSH         = 0x0010,

    DFC_INACTIVE           = 0x0100,
    DFC_PUSHED             = 0x0200,
    DFC_CHECKED            = 0x0400,

    DFC_TRANSPARENT        = 0x0800,
    DFC_HOT                = 0x1000,
    DFC_ADJUSTRECT         = 0x2000,
    DFC_FLAT               = 0x4000,
    DFC_MONO               = 0x8000,
  } // enum DrawFrameControlStateFlags

  #endregion

  #region UxThemeMinButtonStates 

  /// <summary>
  /// UxTheme MinButton States
  /// </summary>
  public enum UxThemeMinButtonStates
  {
    MINBS_NORMAL = 1,
    MINBS_HOT = 2,
    MINBS_PUSHED = 3,
    MINBS_DISABLED = 4,
  } // enum UxThemeMinButtonStates

  #endregion

  #region UxThemeWindowParts 

  /// <summary>
  /// UxTheme Window Parts
  /// </summary>
  public enum UxThemeWindowParts
  {
    WP_CAPTION = 1,
    WP_SMALLCAPTION = 2,
    WP_MINCAPTION = 3,
    WP_SMALLMINCAPTION = 4,
    WP_MAXCAPTION = 5,
    WP_SMALLMAXCAPTION = 6,
    WP_FRAMELEFT = 7,
    WP_FRAMERIGHT = 8,
    WP_FRAMEBOTTOM = 9,
    WP_SMALLFRAMELEFT = 10,
    WP_SMALLFRAMERIGHT = 11,
    WP_SMALLFRAMEBOTTOM = 12,
    WP_SYSBUTTON = 13,
    WP_MDISYSBUTTON = 14,
    WP_MINBUTTON = 15,
    WP_MDIMINBUTTON = 16,
    WP_MAXBUTTON = 17,
    WP_CLOSEBUTTON = 18,
    WP_SMALLCLOSEBUTTON = 19,
    WP_MDICLOSEBUTTON = 20,
    WP_RESTOREBUTTON = 21,
    WP_MDIRESTOREBUTTON = 22,
    WP_HELPBUTTON = 23,
    WP_MDIHELPBUTTON = 24,
    WP_HORZSCROLL = 25,
    WP_HORZTHUMB = 26,
    WP_VERTSCROLL = 27,
    WP_VERTTHUMB = 28,
    WP_DIALOG = 29,
    WP_CAPTIONSIZINGTEMPLATE = 30,
    WP_SMALLCAPTIONSIZINGTEMPLATE = 31,
    WP_FRAMELEFTSIZINGTEMPLATE = 32,
    WP_SMALLFRAMELEFTSIZINGTEMPLATE = 33,
    WP_FRAMERIGHTSIZINGTEMPLATE = 34,
    WP_SMALLFRAMERIGHTSIZINGTEMPLATE = 35,
    WP_FRAMEBOTTOMSIZINGTEMPLATE = 36,
    WP_SMALLFRAMEBOTTOMSIZINGTEMPLATE = 37,
  } // enum UxThemeWindowParts

  #endregion

  #region MoveEventFlags

  /// <summary>
  /// Move event flags
  /// </summary>
  public enum MouseEventFlags
  {
    MOVE        = 0x0001,
    LEFTDOWN    = 0x0002,
    LEFTUP      = 0x0004,
    RIGHTDOWN   = 0x0008,
    RIGHTUP     = 0x0010,
    MIDDLEDOWN  = 0x0020,
    MIDDLEUP    = 0x0040,
    XDOWN       = 0x0080,
    XUP         = 0x0100,
    WHEEL       = 0x0800,
    VIRTUALDESK = 0x4000,
    ABSOLUTE    = 0x8000,
  }

  #endregion

  #region WPF

  public enum WPF
  {
    WPF_SETMINPOSITION        = 0x0001,
    WPF_RESTORETOMAXIMIZED    = 0x0002,
    WPF_ASYNCWINDOWPLACEMENT  = 0x0004,
  }

  #endregion

  #endregion

  #region structures

  #region MSG
   
  /// <summary>
  /// Contains message information from a thread's message queue.
  /// </summary>
  [ StructLayout( LayoutKind.Sequential ) ]
  public struct MSG 
  {
    /// <summary>
    /// Identifies the window whose window procedure receives the message. 
    /// </summary>
    public IntPtr hwnd;
    
    /// <summary>
    /// Specifies the message number. 
    /// </summary>
    public int message;
    
    /// <summary>
    /// Specifies additional information about the message. 
    /// The exact meaning depends on the value of the message member. 
    /// </summary>
    public IntPtr wParam;
    
    /// <summary>
    /// Specifies additional information about the message. 
    /// The exact meaning depends on the value of the message member. 
    /// </summary>
    public IntPtr lParam;
    
    /// <summary>
    /// Specifies the time at which the message was posted. 
    /// </summary>
    public int time;
    
    /// <summary>
    /// Specifies the cursor's x position, in screen coordinates, 
    /// when the message was posted. 
    /// </summary>
    public int pt_x;
    
    /// <summary>
    /// Specifies the cursor's y position, in screen coordinates, 
    /// when the message was posted. 
    /// </summary>
    public int pt_y;
  
  } // struct MSG

  #endregion

  #region PAINTSTRUCT
  
  [ StructLayout( LayoutKind.Sequential ) ]
  public struct PAINTSTRUCT
  {
    /// <summary>
    /// Handle to the display DC to be used for painting.
    /// </summary>
    public IntPtr hdc;
    
    /// <summary>
    /// Specifies whether the background must be erased.
    /// </summary>
    public int fErase;
    
    /// <summary>
    /// Specifies a RECT structure that specifies the upper left and lower right corners of the rectangle 
    /// in which the painting is requested, in device units relative to the upper-left corner of the client area. 
    /// </summary>
    public Rectangle rcPaint;
    
    /// <summary>
    /// Reserved; used internally by the system.
    /// </summary>
    public int fRestore;
        
    /// <summary>
    /// Reserved; used internally by the system.
    /// </summary>
    public int fIncUpdate;
    
    /// <summary>
    /// 32 Bytes Reserved; used internally by the system.
    /// </summary>
    public int Reserved1;
    public int Reserved2;
    public int Reserved3;
    public int Reserved4;
    public int Reserved5;
    public int Reserved6;
    public int Reserved7;
    public int Reserved8;
  } // struct PAINTSTRUCT

  #endregion

  #region RECT

  /// <summary>
  /// The RECT structure defines the coordinates of the upper-left 
  /// and lower-right corners of a rectangle. 
  /// </summary>
  [ StructLayout( LayoutKind.Sequential ) ]
  public struct RECT
  {
    /// <summary>
    /// Specifies the x-coordinate of the upper-left corner of the rectangle.
    /// </summary>
    public int left;
    
    /// <summary>
    /// Specifies the y-coordinate of the upper-left corner of the rectangle.
    /// </summary>
    public int top;
    
    /// <summary>
    /// Specifies the x-coordinate of the lower-right corner of the rectangle.
    /// </summary>
    public int right;
    
    /// <summary>
    /// Specifies the y-coordinate of the lower-right corner of the rectangle.
    /// </summary>
    public int bottom;
  } // struct RECT

  #endregion

  #region POINT
  
  /// <summary>
  /// The POINT structure defines the x- and y- coordinates of a point.
  /// </summary>
  [ StructLayout( LayoutKind.Sequential ) ]
  public struct POINT
  {
    /// <summary>
    /// Specifies the x-coordinate of the point.
    /// </summary>
    public int x;
    
    /// <summary>
    /// Specifies the y-coordinate of the point.
    /// </summary>
    public int y;
  
  } // struct POINT

  #endregion

  #region SCROLLBARINFO
  
  /// <summary>
  /// The SCROLLBARINFO structure contains scroll bar information.
  /// </summary>
  [ StructLayout( LayoutKind.Sequential ) ]
  public struct SCROLLBARINFO
  {
    /// <summary>
    /// Specifies the size, in bytes, of the structure. 
    /// </summary>
    public int cbSize;
    
    /// <summary>
    /// Pointer to a RECT structure that indicates the coordinates of the scroll bar.
    /// </summary>
    public Rectangle rcScrollBar;
    
    /// <summary>
    /// Height or width of the thumb.
    /// </summary>
    public int dxyLineButton;
    
    /// <summary>
    /// Position of the top or left of the thumb. 
    /// </summary>
    public int xyThumbTop;
    
    /// <summary>
    /// Position of the bottom or right of the thumb.
    /// </summary>
    public int xyThumbBottom;
    
    /// <summary>
    /// Reserved.
    /// </summary>
    public int reserved;
    
    /// <summary>
    /// An array of DWORD elements. Each element 
    /// indicates the state of a scroll bar component. 
    /// </summary>
    public int rgState1;
    public int rgState2;
    public int rgState3;
    public int rgState4;
    public int rgState5;
    public int rgState6;
  
  } // struct SCROLLBARINFO

  #endregion

  #region BROWSEINFO

  [ StructLayout( LayoutKind.Sequential, CharSet=CharSet.Auto ), ComVisible( true ) ]
  public struct BROWSEINFO 
  {
    /// <summary>
    /// Handle to the window that owns the dialog box. 
    /// </summary>
    public IntPtr hwndOwner;
    
    /// <summary>
    /// Pointer to an ITEMIDLIST structure (PIDL) that 
    /// specifies the location of the root folder from which to start browsing.
    /// </summary>
    public IntPtr pidlRoot;    
    
    /// <summary>
    /// Pointer to a buffer that receives the display name of the folder that the user selects.
    /// </summary>
    public IntPtr pszDisplayName;    
    
    /// <summary>
    /// Pointer to a null-terminated string that is displayed above 
    /// the tree view control in the dialog box.
    /// </summary>
    public string lpszTitle;    
    
    /// <summary>
    /// Unsigned integer that specifies the options for the dialog box.
    /// </summary>
    public int ulFlags;    
    
    /// <summary>
    /// Pointer to an application-defined function that the dialog box 
    /// calls when an event occurs.
    /// </summary>
    public IntPtr lpfn;    
    
    /// <summary>
    /// Application-defined value that the dialog box passes to the callback 
    /// function, if you specify a callback function.
    /// </summary>
    public IntPtr lParam;    

    /// <summary>
    /// Integer that receives an index for the image associated with the selected folder.
    /// </summary>
    public int iImage;
  } // struct BROWSEINFO 

  #endregion 

  #region COLORREF

  [StructLayout(LayoutKind.Sequential)]
  public struct COLORREF
  {
    public uint m_color;

    public COLORREF(Color color)
    {
      m_color = ((uint)color.R) + 
        (uint)(color.G << 8) +
        (uint)(color.B << 16);
    }

    public Color GetColor()
    {
      return Color.FromArgb( (int)(0x000000FFU | m_color ),
        (int)((0x0000FF00 | m_color) >> 2),
        (int)((0x00FF0000 | m_color) >> 4));
    }

    public void SetColor( Color color )
    {
      m_color = ((uint)color.R) + 
        (uint)(color.G << 8) +
        (uint)(color.B << 16);
    }

    public override string ToString()
    {
      return m_color.ToString();
    }
  }

  #endregion

  #region MARGINS

  [StructLayout(LayoutKind.Sequential)]
  public struct MARGINS 
  {
    public MARGINS(int Left, int Right, int Top, int Bottom)
    {
      cxLeftWidth = Left;
      cxRightWidth = Right;
      cyTopHeight = Top;
      cyBottomHeight = Bottom;
    }

    public int  cxLeftWidth;
    public int  cxRightWidth;
    public int  cyTopHeight;
    public int  cyBottomHeight;
  }

  #endregion

  #region REBARBANDINFO
  
  [StructLayout(LayoutKind.Sequential)]
  public struct REBARBANDINFO
  {
    public void REBARABANDINFO()
    {
      cbSize = (uint)Marshal.SizeOf(this);
      fMask = 0U;
      fStyle = 0U;
      clrFore = new COLORREF(SystemColors.ControlText);
      clrBack = new COLORREF(SystemColors.Control);
      lpText = "";
      cch = 0U;
      iImage = 0;
      hwndChild = IntPtr.Zero;
      cxMinChild = 0U;
      cyMinChild = 0U;
      cx = 0U;
      hbmBack = IntPtr.Zero;
      wID = 0U;
      cyChild = 0U; //Initial Child Height
      cyMaxChild = 0U;
      cyIntegral = 0U;
      cxIdeal = 0U;
      lParam = IntPtr.Zero;
      cxHeader = 0U;
    }

    public uint cbSize;
    public uint fMask;
    public uint fStyle;
    public COLORREF clrFore;
    public COLORREF clrBack;
    public string lpText;
    public uint cch; //Size of text buffer
    public int iImage;
    public IntPtr hwndChild;
    public uint cxMinChild;
    public uint cyMinChild;
    public uint cx;
    public IntPtr hbmBack;
    public uint wID;
    public uint cyChild;
    public uint cyMaxChild;
    public uint cyIntegral;
    public uint cxIdeal;
    public IntPtr lParam;
    public uint cxHeader;
  }

  #endregion

  #region REBARINFO
  
  [StructLayout(LayoutKind.Sequential)]
  public struct REBARINFO
  {
    public uint cbSize;
    public uint fMask;
    public IntPtr himl;
  }

 
  #endregion

  #region INITCOMMONCONTROLSEX

  [StructLayout(LayoutKind.Sequential)]
  public struct INITCOMMONCONTROLSEX 
  {
    public uint dwSize;
    public uint dwICC;
  } 


  #endregion

  #region COLORSCHEME

  [StructLayout(LayoutKind.Sequential)]
  public struct COLORSCHEME
  {
    public uint dwSize;
    public COLORREF clrBtnHighlight;
    public COLORREF clrBtnShadow;
  }

  #endregion

  #region NMHDR

  [StructLayout(LayoutKind.Sequential)]
  public struct NMHDR 
  { 
    public IntPtr hwndFrom; 
    public uint idFrom; 
    public int code; 
  }

  #endregion

  #region NMCUSTOMDRAW

  #region NMCUSTOMDRAW
  [StructLayout(LayoutKind.Sequential)]
  public struct NMCUSTOMDRAW
  {
    public NMHDR hdr;
    public int dwDrawStage;
    public IntPtr hdc;
    public RECT rc;
    public int dwItemSpec;
    public int uItemState;
    public IntPtr lItemlParam;
  }
  #endregion

  #endregion

  #region NMREBARCHILDSIZE

  [StructLayout(LayoutKind.Sequential)]
  public struct NMREBARCHILDSIZE
  {
    public NMHDR hdr;
    public uint uBand;
    public uint wID;
    public RECT rcChild;
    public RECT rcBand;
  }

  #endregion

  #region MOUSEHOOKSTRUCT
  
  [StructLayout(LayoutKind.Sequential)]
  public class MOUSEHOOKSTRUCT 
  {
    public POINT pt;
    public int hwnd;
    public int wHitTestCode;
    public int dwExtraInfo;
  }

  #endregion

  #region TPMPARAMS
  
  public struct TPMPARAMS 
  {
    public int cbSize;
    public RECT rcExclude;
  }

  #endregion

  #region MENUBARINFO
  
  public struct MENUBARINFO 
  {
    public int cbSize;
    public RECT  rcBar;
    public IntPtr hMenu;
    public IntPtr hwndMenu;
    public int fBarFocused;
    public int fFocused;
  }

  #endregion

  #region SHFILEINFO

  [StructLayout(LayoutKind.Sequential)]
  public struct SHFILEINFO
  {
    public SHFILEINFO(bool b)
    {
      hIcon=IntPtr.Zero;iIcon=IntPtr.Zero;dwAttributes=0;szDisplayName="";szTypeName="";
    }
    public IntPtr hIcon;
    public IntPtr iIcon;
    public uint dwAttributes;
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 260)]
    public string szDisplayName;
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 80)]
    public string szTypeName;
  };

  #endregion

  #region ICONINFO

  public struct ICONINFO
  {
    public bool fIcon;
    public int xHotspot;
    public int yHotspot;
    public IntPtr hbmMask;
    public IntPtr hbmColor;
  }

  #endregion

  #region DLLVERSIONINFO
  [StructLayout(LayoutKind.Sequential, CharSet=CharSet.Auto)]
  public struct DLLVERSIONINFO
  {
    public int cbSize;
    public int dwMajorVersion;
    public int dwMinorVersion;
    public int dwBuildNumber;
    public int dwPlatformID;
  }
  #endregion

  #region DLLVERSIONINFO2
  [StructLayout(LayoutKind.Sequential, CharSet=CharSet.Auto)]
  public struct DLLVERSIONINFO2
  {
    public DLLVERSIONINFO info1;
    public int dwFlags;
    ulong ullVersion;
  }
  #endregion

  #region WIN32_FIND_DATA
  [StructLayout(LayoutKind.Sequential, CharSet=CharSet.Auto)]
  public struct WIN32_FIND_DATA
  {
    public uint fileAttributes;
    public FILETIME creationTime;
    public FILETIME lastAccessTime;
    public FILETIME lastWriteTime;
    public uint fileSizeHigh;
    public uint fileSizeLow;
    public uint reserved0;
    public uint reserved1;
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst=260)]
    public string fileName;
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst=14)]
    public string alternateFileName;
  }
  #endregion

  #region SHITEMIDLIST
  [StructLayout(LayoutKind.Sequential)]
  public struct SHITEMIDLIST
  {
    public SHITEMID[] mkid;
  }
  #endregion

  #region SHITEMID
  [StructLayout(LayoutKind.Sequential)]
  public struct SHITEMID
  {
    public ushort cb;
    public byte abID;
  }
  #endregion

  #region SHFILEOPSTRUCT
  /// <summary>
  /// Contains information that the SHFileOperation function uses to perform file operations.
  /// </summary>
  [StructLayout(LayoutKind.Sequential, CharSet=CharSet.Unicode)]
  public struct SHFILEOPSTRUCT
  {
    /// <summary>
    /// Window handle to the dialog box to display information about the status of the file operation.
    /// </summary>
    public IntPtr hwnd;
    /// <summary>
    /// Value that indicates which operation to perform.
    /// </summary>
    public UInt32 wFunc;
    /// <summary>
    /// Address of a buffer to specify one or more source file names.
    /// </summary>
    public IntPtr pFrom;
    /// <summary>
    /// Address of a buffer to contain the name of the destination file or directory.
    /// </summary>
    public IntPtr pTo;
    /// <summary>
    /// Flags that control the file operation (should use FOF).
    /// </summary>
    public UInt16 fFlags;
    /// <summary>
    /// Value that receives TRUE if the user aborted any file operations before they were completed, or FALSE otherwise.
    /// </summary>
    public Int32 fAnyOperationsAborted;
    /// <summary>
    /// A handle to a name mapping object containing the old and new names of the renamed files.
    /// </summary>
    public IntPtr hNameMappings;
    /// <summary>
    /// Address of a string to use as the title of a progress dialog box.
    /// </summary>
    [MarshalAs(UnmanagedType.LPWStr)] public string lpszProgressTitle;
  }
  #endregion

  #region SHELLEXECUTEINFO
  [StructLayoutAttribute(LayoutKind.Sequential)]
  public struct SHELLEXECUTEINFO
  {
    public int cbSize;
    public ShellExecuteFlags fMask;
    public IntPtr hWnd;
    public string lpVerb;
    public string lpFile;
    public string lpParameters;
    public string lpDirectory;
    public ShowWindowStyles nShow;
    public IntPtr hInstApp;
    public IntPtr lpIDList;
    public int lpClass;
    public int hkeyClass;
    public int dwHotKey;
    public IntPtr hIcon;
    public IntPtr hProcess;
  }
  #endregion

  #region STRRET
  [StructLayout(LayoutKind.Sequential)]
  public struct STRRET
  {
    public int uType;
    //		IntPtr pOleStr;
    //		uint uOffset;
    [MarshalAs(UnmanagedType.ByValArray, SizeConst=260)] public byte[] cStr;
  }
  #endregion

  #region MENUITMEINFO
  [StructLayoutAttribute(LayoutKind.Sequential)]
  public struct MENUITMEINFO
  {
    public int cbSize; 
    public MenuItemMasks fMask; 
    public MenuItemTypes fType; 
    public MenuItemStates fState; 
    public int wID; 
    public IntPtr hSubMenu; 
    public IntPtr hbmpChecked; 
    public IntPtr hbmpUnchecked; 
    public int dwItemData; 
    public IntPtr dwTypeData; 
    public int cch; 
    public IntPtr hbmpItem;
  }
  #endregion

  #region StartupInfo
  [StructLayout(LayoutKind.Sequential)]
  public class StartupInfo
  {
    public int cb;
    public String lpReserved;
    public String lpDesktop;
    public String lpTitle;
    public int dwX;
    public int dwY;
    public int dwXSize;
    public int dwYSize;
    public int dwXCountChars;
    public int dwYCountChars;
    public int dwFillAttribute;
    public int dwFlags;
    public UInt16 wShowWindow;
    public UInt16 cbReserved2;
    public Byte  lpReserved2;
    public int hStdInput;
    public int hStdOutput;
    public int hStdError;
  }
  #endregion

  #region ProcessInformation
  [StructLayout(LayoutKind.Sequential)]
  public class ProcessInformation
  {
    public int hProcess;
    public int hThread;
    public int dwProcessId;
    public int dwThreadId;
  }
  #endregion

  #region MENUITEMINFO
  [StructLayout(LayoutKind.Sequential)]
  public struct MENUITEMINFO
  {
    public uint cbSize;
    public uint fMask;
    public uint fType;
    public uint fState;
    public int	wID;
    public int	/*HMENU*/	  hSubMenu;
    public int	/*HBITMAP*/   hbmpChecked;
    public int	/*HBITMAP*/	  hbmpUnchecked;
    public int	/*ULONG_PTR*/ dwItemData;
    public String dwTypeData;
    public uint cch;
    public int /*HBITMAP*/ hbmpItem;
  }
  #endregion

  #region FORMATETC
  [StructLayout(LayoutKind.Sequential)]
  public struct FORMATETC
  {
    public ClipboardFormats	cfFormat;
    public uint ptd;
    public TargetDevices		dwAspect;
    public int			lindex;
    public StorageMediumTypes		tymed;
  }
  #endregion

  #region STGMEDIUM
  [StructLayout(LayoutKind.Sequential)]
  public struct STGMEDIUM
  {
    public uint tymed;
    public uint hGlobal;
    public uint pUnkForRelease;
  }
  #endregion

  #region CMINVOKECOMMANDINFO
  [StructLayout(LayoutKind.Sequential, CharSet=CharSet.Auto)]
  public struct CMINVOKECOMMANDINFO
  {
    public int cbSize;				// sizeof(CMINVOKECOMMANDINFO)
    public int fMask;				// any combination of CMIC_MASK_*
    public IntPtr hwnd;				// might be NULL (indicating no owner window)
    public IntPtr lpVerb;			// either a string or MAKEINTRESOURCE(idOffset)
    public IntPtr lpParameters;		// might be NULL (indicating no parameter)
    public IntPtr lpDirectory;		// might be NULL (indicating no specific directory)
    public int nShow;				// one of SW_ values for ShowWindow() API
    public int dwHotKey;
    public IntPtr hIcon;
  }
  #endregion

  #region LV_ITEM
  [StructLayoutAttribute(LayoutKind.Sequential)]
  public struct LV_ITEM
  {
    public ListViewItemFlags mask;
    public Int32 iItem;
    public Int32 iSubItem;
    public ListViewItemStates state;
    public ListViewItemStates stateMask;
    public String pszText;
    public Int32 cchTextMax;
    public Int32 iImage;
    public IntPtr lParam;
    public Int32 iIndent;
  }
  #endregion

  #region LVITEM
  [StructLayoutAttribute(LayoutKind.Sequential)]
  public struct LVITEM
  {
    public int mask;
    public int iItem;
    public int iSubItem;
    public int state;
    public int stateMask;

    [MarshalAs(UnmanagedType.LPWStr)]
    public string pszText;
    public int cchTextMax;
    public int iImage;
    public int lParam;
    public int iIndent;
    public int iGroupId;
    public int cColumns; // tile view columns
    public int puColumns;
  }
  #endregion

  #region LVGROUP

  [StructLayoutAttribute( LayoutKind.Sequential )] 
  public struct LVGROUP
  { 
    public int cbSize; 
    public int mask; 

    [MarshalAs(UnmanagedType.LPWStr)] 
    public string pszHeader; 
    public int cchHeader; 

    [MarshalAs(UnmanagedType.LPWStr)] 
    public string pszFooter;
    public int cchFooter;
    public int iGroupId; 
    public int stateMask; 
    public int state; 
    public int uAlign; 
  } // LVGROUP

  #endregion

  #region LVCOLUMN
  [StructLayoutAttribute(LayoutKind.Sequential)]
  public struct LVCOLUMN
  {
    public Int32 mask;
    public Int32 fmt;
    public Int32 cx;
    [MarshalAs(UnmanagedType.LPWStr)] 
    public string pszText;
    public Int32 cchTextMax;
    public Int32 iSubItem;
    public Int32 iImage;
    public Int32 iOrder;
  }
  #endregion

  #region LVHITTESTINFO
  [StructLayoutAttribute(LayoutKind.Sequential)]
  public struct LVHITTESTINFO
  {
    public POINTAPI pt;
    public int flags;
    public Int32 iItem;
    public Int32 iSubItem;
  }
  #endregion

  #region NMLVDISPINFO
  [StructLayout(LayoutKind.Sequential, CharSet=CharSet.Auto)]
  public struct NMLVDISPINFO
  {
    public NMHDR hdr;
    public LV_ITEM lvitem;
  }	
  #endregion

  #region NMLISTVIEW
  [StructLayout(LayoutKind.Sequential, CharSet=CharSet.Auto)]
  public struct NMLISTVIEW
  {
    public NMHDR nmhdr;
    public int iItem;
    public int iSubItem;
    public uint uNewState;
    public uint uOldState;
    public uint uChanged;
    public POINTAPI ptAction;
    public IntPtr lParam;
    public bool NewSelected
    {
      get
      {
        return ((ListViewItemStates)uNewState & ListViewItemStates.LVIS_SELECTED) == ListViewItemStates.LVIS_SELECTED;
      }
    }
    public bool OldSelected
    {
      get
      {
        return ((ListViewItemStates)uOldState & ListViewItemStates.LVIS_SELECTED) == ListViewItemStates.LVIS_SELECTED;
      }
    }
    public bool NewCheck
    {
      get
      {
        try
        {
          return uNewState >= 0x1000 ? ((uNewState & (uint)ListViewItemStates.LVIS_STATEIMAGEMASK) >> 12) - 1 > 0 : false;
        }
        catch
        {
          return false;
        }
      }
    }
    public bool OldCheck
    {
      get
      {
        try
        {
          return uOldState >= 0x1000 ? ((uOldState & (uint)ListViewItemStates.LVIS_STATEIMAGEMASK) >> 12) - 1 > 0 : false;
        }
        catch
        {
          return false;
        }
      }
    }
    public bool NewFocused
    {
      get
      {
        return ((ListViewItemStates)uNewState & ListViewItemStates.LVIS_FOCUSED) == ListViewItemStates.LVIS_FOCUSED;
      }
    }
    public bool OldFocused
    {
      get
      {
        return ((ListViewItemStates)uOldState & ListViewItemStates.LVIS_FOCUSED) == ListViewItemStates.LVIS_FOCUSED;
      }
    }
    public bool Select
    {
      get
      {
        return !OldSelected && NewSelected;
      }
    }
    public bool UnSelect
    {
      get
      {
        return OldSelected && !NewSelected;
      }
    }
    public bool Focus
    {
      get
      {
        return !OldFocused && NewFocused;
      }
    }
    public bool UnFocus
    {
      get
      {
        return OldFocused && !NewFocused;
      }
    }
    public bool Check
    {
      get
      {
        return !OldCheck && NewCheck;
      }
    }
    public bool UnCheck
    {
      get
      {
        return OldCheck && !NewCheck;
      }
    }
  }
  #endregion

  #region HDITEM
  [StructLayout(LayoutKind.Sequential, CharSet=CharSet.Auto)]
  public struct HDITEM
  {
    public	HeaderItemFlags mask;
    public	int     cxy;
    public	IntPtr  pszText;
    public	IntPtr  hbm;
    public	int     cchTextMax;
    public	int     fmt;
    public	int     lParam;
    public	int     iImage;      
    public	int     iOrder;
  }	
  #endregion

  #region HD_HITTESTINFO
  [StructLayout(LayoutKind.Sequential)]
  public struct HD_HITTESTINFO 
  {  
    public POINTAPI pt;  
    public HeaderControlHitTestFlags flags; 
    public int iItem; 
  }
  #endregion

  #region POINTAPI
  [StructLayoutAttribute(LayoutKind.Sequential)]
  public struct POINTAPI
  {
    public POINTAPI(System.Drawing.Point p) {x = p.X; y = p.Y;}
    public POINTAPI(Int32 X, Int32 Y) {x = X; y = Y;}
    public Int32 x;
    public Int32 y;
  }
  #endregion

  #region SIZE
  [StructLayout(LayoutKind.Sequential)]
  public struct SIZE
  {
    public int cx;
    public int cy;
  }
  #endregion

  #region NMLVCUSTOMDRAW
  [StructLayout(LayoutKind.Sequential)]
  public struct NMLVCUSTOMDRAW 
  {
    public NMCUSTOMDRAW nmcd;
    public int clrText;
    public int clrTextBk;
    public int iSubItem;
    public int dwItemType;
    public int clrFace;
    public int iIconEffect;
    public int iIconPhase;
    public int iPartId;
    public int iStateId;
    public RECT rcText;
    public uint uAlign;
  } 
  #endregion

  #region NMTVCUSTOMDRAW
  [StructLayout(LayoutKind.Sequential)]
  public struct NMTVCUSTOMDRAW 
  {
    public NMCUSTOMDRAW nmcd;
    public uint clrText;
    public uint clrTextBk;
    public int iLevel;
  }
  #endregion

  #region BITMAPINFO_FLAT
  [StructLayout(LayoutKind.Sequential)]
  public struct BITMAPINFO_FLAT 
  {
    public int      bmiHeader_biSize;
    public int      bmiHeader_biWidth;
    public int      bmiHeader_biHeight;
    public short    bmiHeader_biPlanes;
    public short    bmiHeader_biBitCount;
    public int      bmiHeader_biCompression;
    public int      bmiHeader_biSizeImage;
    public int      bmiHeader_biXPelsPerMeter;
    public int      bmiHeader_biYPelsPerMeter;
    public int      bmiHeader_biClrUsed;
    public int      bmiHeader_biClrImportant;
    [MarshalAs(System.Runtime.InteropServices.UnmanagedType.ByValArray, SizeConst=1024)]
    public byte[] bmiColors; 
  }
  #endregion

  #region BITMAPINFOHEADER
  [StructLayout(LayoutKind.Sequential)]
  public class BITMAPINFOHEADER 
  {
    public int      biSize = Marshal.SizeOf(typeof(BITMAPINFOHEADER));
    public int      biWidth;
    public int      biHeight;
    public short    biPlanes;
    public short    biBitCount;
    public int      biCompression;
    public int      biSizeImage;
    public int      biXPelsPerMeter;
    public int      biYPelsPerMeter;
    public int      biClrUsed;
    public int      biClrImportant;
  }
  #endregion


  #region TRACKMOUSEEVENTS
  [StructLayout(LayoutKind.Sequential)]
  public struct TRACKMOUSEEVENTS
  {
    public uint cbSize;
    public TrackerEventFlags dwFlags;
    public IntPtr hWnd;
    public uint dwHoverTime;
  }
  #endregion

  #region WINDOWPLACEMENT
  [StructLayout(LayoutKind.Sequential, CharSet=CharSet.Auto)]
  public struct WINDOWPLACEMENT
  {	
    public uint length; 
    public uint flags; 
    public uint showCmd; 
    public POINTAPI ptMinPosition; 
    public POINTAPI ptMaxPosition; 
    public RECT  rcNormalPosition; 
  }
  #endregion

  #region SCROLLINFO
  [StructLayout(LayoutKind.Sequential, CharSet=CharSet.Auto)]
  public struct SCROLLINFO
  {
    public 	uint   cbSize;
    public 	uint   fMask;
    public 	int    nMin;
    public 	int    nMax;
    public 	uint   nPage;
    public 	int    nPos;
    public 	int    nTrackPos;
  }
  #endregion

  #region PCOMBOBOXINFO
  [StructLayout(LayoutKind.Sequential, CharSet=CharSet.Auto)]
  public struct PCOMBOBOXINFO
  {
    public uint  cbSize;
    public RECT  rcItem;
    public RECT  rcButton;
    public int   stateButton;
    public IntPtr hwndCombo;
    public IntPtr hwndItem;
    public IntPtr hwndList;
  }
  #endregion

  #region BLENDFUNCTION
  [StructLayout(LayoutKind.Sequential, Pack=1)]
  public struct BLENDFUNCTION
  {
    public byte BlendOp;
    public byte BlendFlags;
    public byte SourceConstantAlpha;
    public byte AlphaFormat;
  }
  #endregion

  #region NMHEADER
  [StructLayout(LayoutKind.Sequential, Pack=1)]
  public struct NMHEADER
  {
    public NMHDR nmhdr;
    public int iItem;
    public int iButton;
    public HDITEM pItem;
  }
  #endregion

  #region INPUT

  [StructLayout(LayoutKind.Sequential, Pack=1)]
  public struct INPUT
  {
    public uint type;
    public int dx;
    public int dy;
    public uint mouseData;
    public uint dwFlags;
    public uint time;
    public uint dwExtra;
  }

  #endregion 

  #endregion
   
  #region User32
  
  /// <summary>
  /// Imported functions from User32.dll.
  /// </summary>
  public class User32
  {
    [ DllImport( "User32.dll", CharSet = CharSet.Auto ) ]
    public static extern bool SetForegroundWindow( IntPtr hwnd );

    /// <summary>
    /// The AnimateWindow function enables you to produce special effects when showing or hiding windows.
    /// </summary>
    /// <param name="hwnd">Handle to the window to animate.</param>
    /// <param name="dwTime">Specifies how long it takes to play the animation, in milliseconds.</param>
    /// <param name="dwFlags">Specifies the type of animation. Must zero or more of the Win32.AnimateFlags values.</param>
    [ DllImport( "User32.dll", CharSet = CharSet.Auto ) ]
    public static extern bool AnimateWindow( IntPtr hwnd, uint dwTime, uint dwFlags );

    /// <summary>
    /// Adds a rectangle to the specified window's update region. 
    /// </summary>
    /// <param name="hwnd">Handle to the window whose update region has changed.</param>
    /// <param name="rect">Pointer to a RECT structure that contains the client coordinates of the rectangle to be added to the update region.</param>
    /// <param name="erase">Specifies whether the background within the update region is to be erased when the update region is processed.</param>
    [ DllImport( "User32.dll", CharSet = CharSet.Auto ) ]
    public static extern bool InvalidateRect( IntPtr hwnd, ref RECT rect, bool erase );

    /// <summary>
    /// Updates the specified rectangle or region in a window's client area.
    /// </summary>
    /// <param name="hwnd">Handle to the window to be redrawn. If this parameter is NULL, the desktop window is updated.</param>
    /// <param name="rect">Pointer to a RECT structure containing the coordinates, in device units, of the update rectangle. This parameter is ignored if the hrgnUpdate parameter identifies a region.</param>
    /// <param name="hrgn">Handle to the update region. If both the hrgnUpdate and lprcUpdate parameters are NULL, the entire client area is added to the update region.</param>
    /// <param name="dwFlags">Specifies one or more RedrawWindowFlags values.</param>
    [ DllImport( "User32.dll", CharSet = CharSet.Auto ) ]
    public static extern bool RedrawWindow( IntPtr hwnd, ref RECT rect, IntPtr hrgn, uint dwFlags );
    
    /// <summary>
    /// The LoadCursor function loads the specified cursor resource from the executable (.EXE) 
    /// file associated with an application instance.
    /// </summary>
    /// <param name="hInstance">Handle to an instance of the module whose executable file contains the cursor to be loaded.</param>
    [ DllImport( "User32.dll", CharSet = CharSet.Auto ) ]
    public static extern IntPtr LoadCursor( IntPtr hInstance, uint cursor );

    /// <summary>
    /// The SetCursor function sets the cursor shape. 
    /// </summary>
    /// <param name="hCursor">Handle to the cursor.</param>
    /// <returns>Handle to previous cursor.</returns>
    [ DllImport( "User32.dll", CharSet = CharSet.Auto ) ]
    public static extern IntPtr SetCursor( IntPtr hCursor );
  
    /// <summary>
    /// The WaitMessage function yields control to other threads when a thread has 
    /// no other messages in its message queue. The WaitMessage function suspends the thread and does 
    /// not return until a new message is placed in the thread's message queue.
    /// </summary>
    [ DllImport( "User32.dll", CharSet = CharSet.Auto ) ]
    public static extern bool WaitMessage();

    /// <summary>
    /// The TranslateMessage function translates virtual-key messages into character messages.
    /// </summary>
    /// <param name="msg">Pointer to an MSG structure that contains message information retrieved from the calling thread's message queue.</param>
    [ DllImport( "User32.dll", CharSet = CharSet.Auto ) ]
    public static extern bool TranslateMessage( ref MSG msg );

    /// <summary>
    /// The DispatchMessage function dispatches a message to a window procedure.
    /// </summary>
    /// <param name="msg">Pointer to an MSG structure that contains message information retrieved from the calling thread's message queue.</param>
    [ DllImport( "User32.dll", CharSet = CharSet.Auto ) ]
    public static extern bool DispatchMessage( ref MSG msg );

    /// <summary>
    /// The PostMessage function places (posts) a message in the message queue associated with the thread 
    /// that created the specified window and returns without waiting for the thread to process the message. 
    /// </summary>
    /// <param name="hwnd">Handle to the window whose window procedure is to receive the message.</param>
    /// <param name="Msg">Specifies the message to be posted.</param>
    /// <param name="wParam">Specifies additional message-specific information.</param>
    /// <param name="lParam">Specifies additional message-specific information.</param>
    [ DllImport( "User32.dll", CharSet = CharSet.Auto ) ]
    public static extern bool PostMessage( IntPtr hwnd, int Msg, uint wParam, uint lParam );

    /// <summary>
    /// The SendMessage function sends the specified message to a window or windows. It calls the 
    /// window procedure for the specified window and does not return until the window procedure 
    /// has processed the message. 
    /// </summary>
    /// <param name="hwnd">Handle to the window whose window procedure is to receive the message.</param>
    /// <param name="Msg">Specifies the message to be posted.</param>
    /// <param name="wParam">Specifies additional message-specific information.</param>
    /// <param name="lParam">Specifies additional message-specific information.</param>
    [ DllImport( "User32.dll", CharSet = CharSet.Auto ) ]
    public static extern uint SendMessage( IntPtr hwnd, int Msg, uint wParam, uint lParam );

		
    [DllImport("User32.dll", CharSet=CharSet.Auto)]
    public static extern uint SendMessage(IntPtr hWnd, int Msg, IntPtr wParam, IntPtr lParam);

    [DllImport("User32.dll", CharSet=CharSet.Auto)]
    public static extern uint SendMessage(IntPtr hWnd, int Msg, int wParam, IntPtr lParam);

    [DllImport("User32.dll", CharSet=CharSet.Auto)]
    public static extern uint SendMessage(IntPtr hWnd, int Msg, int wParam, ref LVGROUP lParam);

    [DllImport("User32.dll", CharSet=CharSet.Auto)]
    public static extern uint SendMessage(IntPtr hWnd, int Msg, int wParam, ref LVITEM lParam);

    [DllImport("User32.dll", CharSet=CharSet.Auto)]
    public static extern uint SendMessage(IntPtr hWnd, int Msg, int wParam, ref LVCOLUMN lParam);
		
    [DllImport("User32.dll", CharSet=CharSet.Auto)]
    public static extern uint SendMessage(IntPtr hWnd, int Msg, int wParam, ref REBARINFO lParam);

    [DllImport("User32.dll", CharSet=CharSet.Auto)]
    public static extern uint SendMessage(IntPtr hWnd, int Msg, int wParam, ref REBARBANDINFO lParam);

    [DllImport("User32.dll", CharSet=CharSet.Auto)]
    public static extern uint SendMessage(IntPtr hWnd, int Msg, int wParam, ref COLORSCHEME lParam);

    [DllImport("User32.dll", CharSet=CharSet.Auto)]
    public static extern uint SendMessage(IntPtr hWnd, int Msg, int wParam, COLORREF lParam);

    [DllImport("User32.dll", CharSet=CharSet.Auto)]
    public static extern uint SendMessage(IntPtr hWnd, int Msg, int wParam, ref RECT lParam);

    [DllImport("User32.dll", CharSet=CharSet.Auto)]
    public static extern uint SendMessage(IntPtr hWnd, int Msg, int wParam, ref MARGINS lParam);

    [DllImport("User32.dll", CharSet=CharSet.Auto)]
    public static extern uint SendMessage(IntPtr hWnd, int Msg, int wParam, int[] lParam);

    /// <summary>
    /// The GetMessage function retrieves a message from the calling thread's message queue. The function 
    /// dispatches incoming sent messages until a posted message is available for retrieval. 
    /// </summary>
    /// <param name="msg">Pointer to an MSG structure that receives message information from the thread's message queue.</param>
    /// <param name="hwnd">Handle to the window whose messages are to be retrieved. </param>
    /// <param name="wFilterMin">Specifies the integer value of the lowest message value to be retrieved.</param>
    /// <param name="wFilterMax">Specifies the integer value of the highest message value to be retrieved.</param>
    [ DllImport( "User32.dll", CharSet = CharSet.Auto ) ]
    public static extern bool GetMessage( ref MSG msg, int hwnd, uint wFilterMin, uint wFilterMax );

    /// <summary>
    /// The PeekMessage function dispatches incoming sent messages, checks the thread message queue for a 
    /// posted message, and retrieves the message.
    /// </summary>
    /// <param name="msg">Pointer to an MSG structure that receives message information.</param>
    /// <param name="hwnd">Handle to the window whose messages are to be examined.</param>
    /// <param name="wFilterMin">Specifies the value of the first message in the range of messages to be examined.</param>
    /// <param name="wFilterMax">Specifies the value of the last message in the range of messages to be examined.</param>
    /// <param name="wFlag">Specifies how messages are handled.</param>
    [ DllImport( "User32.dll", CharSet = CharSet.Auto ) ]
    public static extern bool PeekMessage( ref MSG msg, int hwnd, uint wFilterMin, uint wFilterMax, uint wFlag );

    /// <summary>
    ///  prepares the specified window for painting and fills a PAINTSTRUCT structure with information about the painting. 
    /// </summary>
    /// <param name="hwnd">Handle to the window to be repainted.</param>
    /// <param name="ps">Pointer to the PAINTSTRUCT structure that will receive painting information.</param>
    [ DllImport( "User32.dll", CharSet = CharSet.Auto ) ]
    public static extern IntPtr BeginPaint( IntPtr hwnd, ref PAINTSTRUCT ps );

    /// <summary>
    /// The EndPaint function marks the end of painting in the specified window. This function is required for each call to the 
    /// BeginPaint function, but only after painting is complete. 
    /// </summary>
    /// <param name="hwnd">Handle to the window that has been repainted.</param>
    /// <param name="ps">Pointer to a PAINTSTRUCT structure that contains the painting information retrieved by BeginPaint.</param>
    [ DllImport( "User32.dll", CharSet = CharSet.Auto ) ]
    public static extern bool EndPaint( IntPtr hwnd, ref PAINTSTRUCT ps );

    /// <summary>
    /// Sets the specified window's show state.
    /// </summary>
    /// <param name="hwnd">Handle to the window.</param>
    /// <param name="cmdShow">Specifies how the window is to be shown. Must be one of the ShowWindowStyles values.</param>
    [ DllImport( "User32.dll", CharSet = CharSet.Auto ) ]
    public static extern int ShowWindow( IntPtr hwnd, short cmdShow );
 
    /// <summary>
    /// Changes the size, position, and Z order of a child, pop-up, or top-level window.
    /// </summary>
    /// <param name="hwnd">Handle to the window.</param>
    /// <param name="hWndAfter">Handle to the window to precede the positioned window in the Z order.</param>
    /// <param name="X">Specifies the new position of the left side of the window, in client coordinates.</param>
    /// <param name="Y">Specifies the new position of the top of the window, in client coordinates.</param>
    /// <param name="Width">Specifies the new width of the window, in pixels.</param>
    /// <param name="Height">Specifies the new height of the window, in pixels.</param>
    /// <param name="flags">Specifies the window sizing and positioning flags. </param>
    [ DllImport( "User32.dll", CharSet = CharSet.Auto ) ]
    public static extern int SetWindowPos( IntPtr hwnd, IntPtr hWndAfter, int X, int Y, int Width, int Height, uint flags );

    /// <summary>
    /// Converts the client-area coordinates of a specified point to screen coordinates.
    /// </summary>
    /// <param name="hwnd">Handle to the window whose client area is used for the conversion.</param>
    /// <param name="pt">Pointer to a POINT structure that contains the client coordinates to be converted.</param>
    [ DllImport( "User32.dll", CharSet = CharSet.Auto ) ]
    public static extern bool ClientToScreen(IntPtr hwnd, ref POINT pt);

    /// <summary>
    /// Converts the screen coordinates of a specified point on the screen 
    /// to client-area coordinates. 
    /// </summary>
    /// <param name="hwnd">Handle to the window whose client area will be used for the conversion.</param>
    /// <param name="pt">Pointer to a POINT structure that specifies the screen coordinates to be converted.</param>
    [ DllImport( "User32.dll", CharSet = CharSet.Auto ) ]
    public static extern bool ScreenToClient( IntPtr hwnd, ref POINT pt );

    /// <summary>
    /// Removes the caret from the screen. Hiding a caret does not destroy 
    /// its current shape or invalidate the insertion point. 
    /// </summary>
    /// <param name="hwnd">Handle to the window that owns the caret.</param>
    [ DllImport( "User32.dll", CharSet = CharSet.Auto ) ]
    public static extern bool HideCaret( IntPtr hwnd );

    /// <summary>
    /// Makes the caret visible on the screen at the caret's current position. 
    /// When the caret becomes visible, it begins flashing automatically. 
    /// </summary>
    /// <param name="hwnd">Handle to the window that owns the caret.</param>
    [ DllImport( "User32.dll", CharSet = CharSet.Auto ) ]
    public static extern bool ShowCaret( IntPtr hwnd );

    /// <summary>
    /// Retrieves information about the specified scroll bar.
    /// </summary>
    /// <param name="hwnd">Handle to a window associated with the scroll bar whose information is to be retrieved.</param>
    /// <param name="idObject">Specifies the scroll bar object.</param>
    /// <param name="psbi">Pointer to a SCROLLBARINFO structure to receive the information.</param>
    [ DllImport( "User32.dll", CharSet = CharSet.Auto ) ]
    public static extern bool GetScrollBarInfo( IntPtr hwnd, int idObject, ref SCROLLBARINFO psbi );

    /// <summary>
    /// The GetSystemMetrics function retrieves various system metrics (widths and heights 
    /// of display elements) and system configuration settings. All dimensions retrieved by 
    /// GetSystemMetrics are in pixels.
    /// </summary>
    /// <param name="nIndex">System metric or configuration setting to retrieve. Use one of the SystemMetricsFlags values.</param>
    [ DllImport( "User32.dll", CharSet = CharSet.Auto ) ]
    public static extern int GetSystemMetrics( int nIndex );

    /// <summary>
    /// Creates a window.
    /// </summary>
    /// <param name="dwExStyle">extended window style</param>
    /// <param name="lpClassName">registered class name</param>
    /// <param name="lpWindowName">window name</param>
    /// <param name="dwStyle">window style</param>
    /// <param name="x">horizontal position of window</param>
    /// <param name="y">vertical position of window</param>
    /// <param name="nWidth">window width</param>
    /// <param name="nHeight">window height</param>
    /// <param name="hWndParent">handle to parent or owner window</param>
    /// <param name="hMenu">menu handle or child identifier</param>
    /// <param name="hInstance">handle to application instance</param>
    /// <param name="lpParam">window-creation data</param>
    /// <returns>Window handle</returns>
    [ DllImport( "User32.dll", CharSet=CharSet.Auto ) ]
    public static extern IntPtr CreateWindowEx( uint dwExStyle, string lpClassName, string lpWindowName,uint dwStyle, int x, int y, int nWidth, int nHeight, IntPtr hWndParent, IntPtr hMenu, IntPtr hInstance, int lpParam );

    /// <summary>
    /// Retuns the windows rectangle for the specified window handle.
    /// </summary>
    /// <param name="hWnd">handle to window</param>
    /// <param name="lpRect">window coordinates</param>
    /// <returns>True if successfull; false otherwise</returns>
    [ DllImport( "User32.dll", CharSet=CharSet.Auto ) ]
    public static extern bool GetWindowRect( IntPtr hWnd, ref RECT lpRect );
		
    /// <summary>
    /// 
    /// </summary>
    /// <param name="hWnd">handle to window</param>
    /// <param name="X">horizontal position</param>
    /// <param name="Y">vertical position</param>
    /// <param name="nWidth">width</param>
    /// <param name="nHeight">height</param>
    /// <param name="bRepaint">repaint option</param>
    /// <returns>0 on success</returns>
    [DllImport("User32.dll", CharSet=CharSet.Auto)]
    public static extern int MoveWindow( IntPtr hWnd, int X, int Y, int nWidth, int nHeight, bool bRepaint );

    [ DllImport("User32.dll", CharSet = CharSet.Auto, EntryPoint = "SetWindowLong", ExactSpelling=false) ]
    public static extern IntPtr SetWindowLongPtr( IntPtr hWnd, int nIndex, IntPtr dwNewLong );

    public delegate int HookProc( int nCode, Int32 wParam, IntPtr lParam );

    [DllImport("user32.dll",CharSet=CharSet.Auto, CallingConvention=CallingConvention.StdCall)]
    public static extern int SetWindowsHookEx( int idHook, HookProc lpfn, IntPtr hInstance, int threadId );

    [DllImport("user32.dll",CharSet=CharSet.Auto, CallingConvention=CallingConvention.StdCall)]
    public static extern bool UnhookWindowsHookEx( int idHook );
		
    [DllImport("user32.dll",CharSet=CharSet.Auto, CallingConvention=CallingConvention.StdCall)]
    public static extern int CallNextHookEx( int idHook, int nCode, Int32 wParam, IntPtr lParam );  

    [DllImport("user32")] 
    public static extern int TrackPopupMenu( IntPtr hMenu, int un, int n1, int n2, int r, IntPtr hwnd, IntPtr prc );

    [DllImport("user32")] 
    public static extern int TrackPopupMenuEx( IntPtr hMenu, int un, int n1, int n2, IntPtr hwnd, ref TPMPARAMS lpTPMParams);

    [DllImport("user32")] 
    public static extern int GetMenuBarInfo( IntPtr hwnd, int idObject, int idItem, ref MENUBARINFO pmbi );

    [DllImport("user32")] 
    public static extern int  GetMenuItemRect( IntPtr hWnd, IntPtr hMenu, int uItem, ref RECT lprcItem );

    [DllImport("user32")] 
    public static extern int  EndMenu();

    [DllImport("user32")] 
    public static extern IntPtr GetDC( IntPtr hwnd );

    [DllImport("user32")] 
    public static extern int ReleaseDC( IntPtr hwnd, IntPtr hdc );

    [ DllImport( "user32" ) ]
    public static extern int RegisterWindowMessage( string name );

    [ DllImport( "user32" ) ]
    public static extern int SendNotifyMessage( int hWnd, int Msg, int wParam, int lParam );

    [DllImport("user32")]
    public static extern int DestroyIcon( IntPtr hIcon );

    [DllImport("user32.dll")]
    public static extern int GetIconInfo( IntPtr hIcon, out ICONINFO piconinfo );


    ///


    [DllImport("user32.dll", CharSet=CharSet.Auto)]
    static public extern uint SendInput(uint nInputs, INPUT[] inputs, int cbSize);

    [DllImport("user32.dll", CharSet=CharSet.Auto)]
    static public extern bool ScreenToClient(IntPtr hWnd, ref POINTAPI point);

    [DllImport("user32.dll", CharSet=CharSet.Auto)]
    static public extern int DrawFrameControl(IntPtr hDc, RECT rect, DrawFrameControlFlags nType, DrawFrameControlStateFlags nState);

    [DllImport("user32.dll", CharSet=CharSet.Auto)]
    static public extern int GetSysColor(SystemColors nIndex);

    [DllImport("user32.dll", CharSet=CharSet.Auto)]
    static public extern bool GetComboBoxInfo(IntPtr hWnd, ref PCOMBOBOXINFO cbi);

    [DllImport("user32.dll", CharSet=CharSet.Auto)]
    static public extern bool DestroyWindow(IntPtr hWnd);

    [DllImport("user32.dll", CharSet=CharSet.Auto)]
    static public extern IntPtr GetDesktopWindow();

    [DllImport("user32.dll", CharSet=CharSet.Auto)]
    static public extern bool ShowWindow(IntPtr hWnd, ShowWindowStyles State);

    [DllImport("user32.dll", CharSet=CharSet.Auto)]
    static public extern bool UpdateWindow(IntPtr hWnd);


    [DllImport("user32.dll", CharSet=CharSet.Auto)]
    static public extern bool OpenClipboard(IntPtr hWndNewOwner);

    [DllImport("user32.dll", CharSet=CharSet.Auto)]
    static public extern bool CloseClipboard();

    [DllImport("user32.dll", CharSet=CharSet.Auto)]
    static public extern bool EmptyClipboard();

    [DllImport("user32.dll", CharSet=CharSet.Auto)]
    static public extern IntPtr SetClipboardData( uint Format, IntPtr hData);

    [DllImport("user32.dll", CharSet=CharSet.Auto)]
    static public extern bool GetMenuItemRect(IntPtr hWnd, IntPtr hMenu, uint Item, ref RECT rc);

    [DllImport("user32.dll", ExactSpelling=true, CharSet=CharSet.Auto)]
    public static extern IntPtr GetParent(IntPtr hWnd);

    [DllImport("user32.dll", CharSet=CharSet.Auto)]
    public static extern int SendMessage(IntPtr hWnd, HeaderControlMessages msg, int wParam, ref HDITEM lParam);

    [DllImport("user32.dll", CharSet=CharSet.Auto)]
    public static extern int SendMessage(IntPtr hWnd, int msg, int wParam, int lParam);

    [DllImport("user32.dll", CharSet=CharSet.Auto)]
    public static extern int SendMessage(IntPtr hWnd, HeaderControlMessages msg, IntPtr wParam, ref HD_HITTESTINFO hd_hittestinfo);

    [DllImport("user32.dll", CharSet=CharSet.Auto)]
    public static extern int SendMessage(IntPtr hWnd, WindowMessages msg, int wParam, int lParam);

    [DllImport("user32.dll", CharSet=CharSet.Auto)]
    public static extern void SendMessage(IntPtr hWnd, int msg, IntPtr wParam, ref NMHDR lParam);

    [DllImport("user32.dll", CharSet=CharSet.Auto)]
    public static extern void SendMessage(IntPtr hWnd, int msg, IntPtr wParam, ref NMLVDISPINFO lParam);

    [DllImport("user32.dll", CharSet=CharSet.Auto)]
    public static extern int SendMessage(IntPtr hWnd, int msg, int wParam, ref POINTAPI lParam);

    [DllImport("user32.dll", CharSet=CharSet.Auto)]
    public static extern IntPtr PostMessage(IntPtr hWnd, int msg, int wParam, int lParam);

    [DllImport("user32.dll", CharSet=CharSet.Auto)]
    public static extern IntPtr PostMessage(IntPtr hWnd, WindowMessages msg, int wParam, int lParam);

    [DllImport("user32.dll", CharSet=CharSet.Auto)]
    public static extern IntPtr SetFocus(IntPtr hWnd);

    [DllImport("user32.dll", CharSet=CharSet.Auto)]
    public extern static int DrawText(IntPtr hdc, string lpString, int nCount, ref RECT lpRect, DrawTextFormatFlags flags);

    [DllImport("user32.dll", CharSet=CharSet.Auto)]
    public extern static IntPtr SetParent(IntPtr hChild, IntPtr hParent);

    [DllImport("user32.dll", CharSet=CharSet.Auto)]
    public extern static IntPtr GetDlgItem(IntPtr hDlg, int nControlID);

    [DllImport("user32.dll", CharSet=CharSet.Auto)]
    public extern static int GetClientRect(IntPtr hWnd, ref RECT rc);

    [DllImport("user32.dll", CharSet=CharSet.Auto)]
    public extern static int InvalidateRect(IntPtr hWnd,  ref RECT rc, int bErase);

    [DllImport("user32.dll", CharSet=CharSet.Auto)]
    public extern static int InvalidateRect(IntPtr hWnd,  IntPtr rc, int bErase);

    [DllImport("User32.dll", CharSet=CharSet.Auto)]
    public static extern bool PeekMessage(ref MSG msg, int hWnd, uint wFilterMin, uint wFilterMax, PeekMessageFlags flags);

    [DllImport("User32.dll", CharSet=CharSet.Auto)]
    public static extern IntPtr LoadCursor(IntPtr hInstance, CursorTypes cursor);

    [DllImport("User32.dll", CharSet=CharSet.Auto)]
    public static extern IntPtr GetFocus();

    [DllImport("User32.dll", CharSet=CharSet.Auto)]
    public static extern bool ReleaseCapture();

    [DllImport("User32.dll", CharSet=CharSet.Auto)]
    public static extern IntPtr SetCapture(IntPtr hWnd);

    [DllImport("User32.dll", CharSet=CharSet.Auto)]
    public static extern bool UpdateLayeredWindow(IntPtr hwnd, IntPtr hdcDst, ref POINTAPI pptDst, ref SIZE psize, 
      IntPtr hdcSrc, ref POINTAPI pprSrc, Int32 crKey, ref BLENDFUNCTION pblend, UpdateLayeredWindowFlags dwFlags);

    [DllImport("User32.dll", CharSet=CharSet.Auto)]
    public static extern bool ClientToScreen(IntPtr hWnd, ref POINTAPI pt);

    [DllImport("User32.dll", CharSet=CharSet.Auto)]
    public static extern bool TrackMouseEvent(ref TRACKMOUSEEVENTS tme);

    [DllImport("User32.dll", CharSet=CharSet.Auto)]
    public static extern bool SetWindowRgn(IntPtr hWnd, IntPtr hRgn, bool redraw);

    [DllImport("User32.dll", CharSet=CharSet.Auto)]
    public static extern ushort GetKeyState(int virtKey);

    [DllImport("User32.dll", CharSet=CharSet.Auto)]
    public static extern int GetClassName(IntPtr hWnd, System.Text.StringBuilder ClassName, int nMaxCount);

    [DllImport("User32.dll", CharSet=CharSet.Auto)]
    public static extern IntPtr GetDCEx(IntPtr hWnd, IntPtr hRegion, uint flags);

    [DllImport("User32.dll", CharSet=CharSet.Auto)]
    public static extern IntPtr GetWindowDC(IntPtr hWnd);

    [DllImport("User32.dll", CharSet=CharSet.Auto)]
    public static extern int FillRect(IntPtr hDC, ref RECT rect, IntPtr hBrush);


    [DllImport("User32.dll", CharSet=CharSet.Auto)]
    public static extern int SetWindowText(IntPtr hWnd, string text);

    [DllImport("User32.dll", CharSet=CharSet.Auto)]
    public static extern int GetWindowText(IntPtr hWnd, System.Text.StringBuilder text, int maxCount);

    [DllImport("user32.dll", CharSet=CharSet.Auto)] 
    static public extern IntPtr SetClipboardViewer(IntPtr hWndNewViewer); 

    [DllImport("user32.dll", CharSet=CharSet.Auto)]
    static public extern int ChangeClipboardChain(IntPtr hWndRemove, IntPtr hWndNewNext);

    [DllImport("user32.dll", CharSet=CharSet.Auto)]
    static public extern int GetSystemMetrics(SystemMetricsCodes code);

    [DllImport("user32.dll", CharSet=CharSet.Auto)]
    static public extern int SetScrollInfo(IntPtr hwnd,  int bar, ref SCROLLINFO si, int fRedraw);

    [DllImport("user32.dll", CharSet=CharSet.Auto)]
    static public extern int ShowScrollBar(IntPtr hWnd, int bar,  int show);

    [DllImport("user32.dll", CharSet=CharSet.Auto)]
    static public extern int EnableScrollBar(IntPtr hWnd, uint flags, uint arrows);

    [DllImport("user32.dll", CharSet=CharSet.Auto)]
    static public extern int BringWindowToTop(IntPtr hWnd);

    [DllImport("user32.dll", CharSet=CharSet.Auto)]
    static public extern int GetScrollInfo(IntPtr hwnd, int bar, ref SCROLLINFO si);

    [DllImport("user32.dll", CharSet=CharSet.Auto)]
    static public extern int ScrollWindowEx(IntPtr hWnd, int dx, int dy, 
      ref RECT rcScroll, ref RECT rcClip, IntPtr UpdateRegion, ref RECT rcInvalidated, uint flags);

    [DllImport("user32.dll", CharSet=CharSet.Auto)]
    static public extern bool IsWindow(IntPtr hWnd);

    [DllImport("user32.dll", CharSet=CharSet.Auto)]
    static public extern int LockWindowUpdate(IntPtr hWnd);

    [DllImport("user32.dll", CharSet=CharSet.Auto)]
    static public extern bool ValidateRect(IntPtr hWnd, ref RECT rcInvalidated);

    [DllImport("user32.dll", CharSet=CharSet.Auto)]
    static public extern bool ValidateRect(IntPtr hWnd, IntPtr rc);

    [DllImport("user32.dll", CharSet=CharSet.Auto)]
    static public extern int GetScrollBarInfo(IntPtr hWnd, SystemObjects id, ref SCROLLBARINFO sbi);

    [DllImport("user32.dll", CharSet=CharSet.Auto)]
    static public extern int SetProp(IntPtr hWnd, IntPtr atom, IntPtr hData);

    [DllImport("user32.dll", CharSet=CharSet.Auto)]
    static public extern int CallWindowProc(IntPtr hOldProc, IntPtr hWnd, uint message, int wParam, int lParam);

    [DllImport("user32.dll")]
    public static extern bool SendMessage(IntPtr hWnd, ListViewMessages msg,
      Int32 wParam, ref LV_ITEM lParam);

    [DllImport("user32.dll")]
    public static extern int SendMessage(IntPtr hWnd, Int32 msg, Int32 wParam, ref LVHITTESTINFO lParam);

    [DllImport("user32.dll")]
    public static extern bool SendMessage(IntPtr hWnd, int msg, IntPtr wParam, ref IntPtr lParam);

    [DllImport("user32.dll")]
    public static extern bool AllowSetForegroundWindow( int dwProcessId );

    [DllImport("user32.dll")]
    public static extern uint SetWindowPlacement( IntPtr hWnd, ref WINDOWPLACEMENT lpwp );

    [DllImport("User32.dll")]
    public static extern uint GetWindowPlacement( IntPtr hWnd, ref WINDOWPLACEMENT wp );

    [DllImport("User32.dll")]
    public static extern uint IsWindowVisible( IntPtr hWnd );


  } // class User32

  
  #endregion
  
  #region Shell

  /// <summary>
  /// Implements shell interfaces
  /// </summary>
  public class Shell
  {
    #region GetShellFolder function
    public static COMInterfaces.IShellFolder GetShellFolder(IntPtr handle, string path, out IntPtr FolderID)
    {
      string tempPath = path;

      // Get the desktop folder
      COMInterfaces.IShellFolder returnedShellFolder = null;
      Shell.SHGetDesktopFolder(out returnedShellFolder);

      System.Guid IID_IShellFolder = new System.Guid("000214E6-0000-0000-C000-000000000046");
      IntPtr Pidl = IntPtr.Zero;
      uint i,j=0;
      // Connect to "My Computer"
      Shell.SHGetSpecialFolderLocation(handle, ShellSpecialFolders.CSIDL_DRIVES, out Pidl);
      COMInterfaces.IShellFolder tempfolder;
      returnedShellFolder.BindToObject(Pidl, IntPtr.Zero, ref IID_IShellFolder, out tempfolder);
      Marshal.ReleaseComObject(returnedShellFolder);
      returnedShellFolder = tempfolder;
      int lastindexposition = 0;
      while(lastindexposition != -1)
      {
        lastindexposition = tempPath.IndexOf('\\');
        if(lastindexposition == -1) break;
        string foldername = tempPath.Remove(lastindexposition, tempPath.Length - lastindexposition) + @"\";
        returnedShellFolder.ParseDisplayName(handle, IntPtr.Zero, foldername, out i, out Pidl, ref j);
        returnedShellFolder.BindToObject(Pidl, IntPtr.Zero, ref IID_IShellFolder, out tempfolder);
        Marshal.ReleaseComObject(returnedShellFolder);
        returnedShellFolder = tempfolder;
        tempPath = tempPath.Substring(++lastindexposition);
      }
      FolderID = Pidl;
      return(returnedShellFolder);
    }
    #endregion

    #region Copy, Delete, Move, Rename
    public static bool DoOperation(IntPtr Handle, string[] source, string dest, FileOperations operation)
    {
      SHFILEOPSTRUCT fileop = new SHFILEOPSTRUCT();
      fileop.hwnd = Handle;
      fileop.lpszProgressTitle = Enum.GetName(typeof(FileOperations), operation);
      fileop.wFunc = (uint) operation;
      fileop.pFrom = Marshal.StringToHGlobalUni(StringArrayToMultiString(source));
      fileop.pTo = Marshal.StringToHGlobalUni(dest);
      fileop.fAnyOperationsAborted = 0;
      fileop.hNameMappings = IntPtr.Zero;

      return SHFileOperation(ref fileop) == 0;
    }
    private static String StringArrayToMultiString(String[] stringArray)
    {
      String multiString = "";

      if (stringArray == null)
        return "";

      for (int i=0 ; i<stringArray.Length ; i++)
        multiString += stringArray[i] + '\0';
    
      multiString += '\0';
    
      return multiString;
    }
    #endregion

    #region Properties Launch
    public static int ViewFileProperties(string path)
    {
      if(!System.IO.File.Exists(path)) return(-1);
      SHELLEXECUTEINFO info = new SHELLEXECUTEINFO();
      info.cbSize = Marshal.SizeOf(typeof(SHELLEXECUTEINFO));
      info.fMask = ShellExecuteFlags.SEE_INVOKEIDLIST;
      info.hWnd = IntPtr.Zero;
      info.lpVerb = "properties";
      info.lpFile = System.IO.Path.GetFileName(path);
      info.lpParameters = "";
      info.lpDirectory = System.IO.Path.GetDirectoryName(path);
      info.nShow = ShowWindowStyles.SW_SHOW;
      info.hInstApp = IntPtr.Zero;
      IntPtr ptr = Marshal.AllocCoTaskMem(Marshal.SizeOf(typeof(SHELLEXECUTEINFO)));
      Marshal.StructureToPtr(info, ptr, false);
      return(ShellExecuteEx(ptr));
    }
    public static int ViewDirectoryProperties(string path)
    {
      if(!System.IO.Directory.Exists(path)) return(-1);
      SHELLEXECUTEINFO info = new SHELLEXECUTEINFO();
      info.cbSize = Marshal.SizeOf(typeof(SHELLEXECUTEINFO));
      info.fMask = ShellExecuteFlags.SEE_INVOKEIDLIST;
      info.hWnd = IntPtr.Zero;
      info.lpVerb = "properties";
      info.lpFile = "";
      info.lpParameters = "";
      info.lpDirectory = path;
      info.nShow = ShowWindowStyles.SW_SHOW;
      info.hInstApp = IntPtr.Zero;
      IntPtr ptr = Marshal.AllocCoTaskMem(Marshal.SizeOf(typeof(SHELLEXECUTEINFO)));
      Marshal.StructureToPtr(info, ptr, false);
      return(ShellExecuteEx(ptr));
    }
    #endregion

    #region GetIcon
    static uint FILE_ATTRIBUTE_DIRECTORY        = 0x00000010;
    static uint FILE_ATTRIBUTE_NORMAL           = 0x00000080;
    public static System.Drawing.Icon GetFileIcon(string strPath, bool bSmall)
    {
      SHFILEINFO info = new SHFILEINFO(true);
      ShellGetFileInformationFlags flags =
        ShellGetFileInformationFlags.SHGFI_ICON |
        (System.IO.File.Exists(strPath) ? 0 : ShellGetFileInformationFlags.SHGFI_USEFILEATTRIBUTES) |
        (bSmall ? ShellGetFileInformationFlags.SHGFI_SMALLICON : ShellGetFileInformationFlags.SHGFI_LARGEICON);
      SHGetFileInfo(strPath,
        FILE_ATTRIBUTE_NORMAL,
        out info,
        (uint)Marshal.SizeOf(info),
        flags);
      System.Drawing.Icon icon = System.Drawing.Icon.FromHandle(info.hIcon);
      return icon;
    }
    public static System.Drawing.Icon GetFolderIcon(string strPath, bool bSmall, bool bOpen)
    {
      SHFILEINFO info = new SHFILEINFO(true);
      ShellGetFileInformationFlags flags =
        ShellGetFileInformationFlags.SHGFI_ICON |
        (bSmall ? ShellGetFileInformationFlags.SHGFI_SMALLICON : ShellGetFileInformationFlags.SHGFI_LARGEICON) |
        (bOpen ? ShellGetFileInformationFlags.SHGFI_OPENICON : 0) |
        (System.IO.Directory.Exists(strPath) ? 0 : ShellGetFileInformationFlags.SHGFI_USEFILEATTRIBUTES);
      SHGetFileInfo(strPath,
        FILE_ATTRIBUTE_DIRECTORY,
        out info,
        (uint)Marshal.SizeOf(info),
        flags);
      System.Drawing.Icon icon = System.Drawing.Icon.FromHandle(info.hIcon);
      return icon;
    }
    #endregion

    #region SHParseDisplayName
    [DllImport("shell32.dll")]
    public static extern Int32 SHParseDisplayName(
      [MarshalAs(UnmanagedType.LPWStr)]
      String pszName,
      IntPtr pbc,
      out IntPtr ppidl,
      UInt32 sfgaoIn,
      out UInt32 psfgaoOut);
    #endregion

    #region SHGetDataFromIDList
    [DllImport("shell32.dll", CharSet=CharSet.Auto)]
    public static extern uint SHGetDataFromIDList(
      COMInterfaces.IShellFolder psf,
      IntPtr pidl,
      ShellFolderGetaFromIDList nFormat,
      out WIN32_FIND_DATA pv,
      int cb);
    #endregion

    #region SHGetPathFromIDList
    [DllImport("Shell32.Dll", CharSet=CharSet.Auto)]
    [return:MarshalAs(UnmanagedType.Bool)]
    public static extern Boolean SHGetPathFromIDList(
      IntPtr pidl,
      [In,Out,MarshalAs(UnmanagedType.LPTStr)] String pszPath);
    #endregion

    #region SHGetSpecialFolderPath
    [DllImport("Shell32.Dll", CharSet=CharSet.Auto)]
    [return:MarshalAs(UnmanagedType.Bool)]
    public static extern Boolean SHGetSpecialFolderPath(
      IntPtr hwndOwner,
      [In,Out,MarshalAs(UnmanagedType.LPTStr)] String pszPath,
      ShellSpecialFolders nFolder,
      [In,MarshalAs(UnmanagedType.Bool)] Boolean fCreate);
    #endregion

    #region SHGetDesktopFolder
    /// <summary>
    /// Retrieves the IShellFolder interface for the desktop folder, which is the root of the Shell's namespace.
    /// </summary>
    [DllImport("shell32.dll")]
    public static extern int SHGetDesktopFolder(out COMInterfaces.IShellFolder folder);
    #endregion

    #region SHGetSpecialFolderLocation
    /// <summary>
    /// Retrieves a pointer to the ITEMIDLIST structure of a special folder.
    /// </summary>
    [DllImport("shell32.dll")]
    public static extern int SHGetSpecialFolderLocation(
      IntPtr handle,
      ShellSpecialFolders nFolder,
      out IntPtr ppidl);
    #endregion

    #region SHBindToParent
    /// <summary>
    /// This function takes the fully-qualified PIDL of a namespace object, and returns a specified interface pointer on the parent object.
    /// </summary>
    [DllImport("shell32.dll")]
    public static extern int SHBindToParent(
      IntPtr pidl,
      Guid iid,
      out COMInterfaces.IShellFolder folder,
      out IntPtr pidlRelative);
    #endregion

    #region SHFileOperation
    /// <summary>
    /// Copies, moves, renames, or deletes a file system object.
    /// </summary>
    [DllImport("shell32.dll", CharSet=CharSet.Unicode)]
    public static extern Int32 SHFileOperation(
      ref SHFILEOPSTRUCT FileOp);
    #endregion

    #region SHGetFileInfo
    [DllImport("Shell32.dll")]
    public static extern int SHGetFileInfo(
      string pszPath,
      uint dwFileAttributes,
      out SHFILEINFO psfi,
      uint cbfileInfo,
      ShellGetFileInformationFlags uFlags);
    #endregion

    #region ShellExecute
    /// <summary>
    /// Execute the file
    /// </summary>
    [DllImport("Shell32.dll")]
    public static extern int ShellExecute
      (IntPtr Hwnd,
      string strOperation,
      string strFile,
      string strParametres,
      string strDirectory,
      int ShowCmd);
    #endregion

    #region ShellExecuteEx
    [DllImport("Shell32.dll")]
    public static extern int ShellExecuteEx(IntPtr infos);
    #endregion

    #region StrRetToBuf
    /// <summary>
    /// Takes a STRRET structure returned by IShellFolder::GetDisplayNameOf, converts it to a string, and places the result in a buffer.
    /// </summary>
    [DllImport("Shlwapi.Dll", CharSet=CharSet.Auto)]
    public static extern uint StrRetToBuf(
      STRRET pstr,
      IntPtr pidl,
      [In,Out,MarshalAs(UnmanagedType.LPTStr)] String pszBuf,
      uint cchBuf);
    #endregion

  } // class Shell

  #endregion

  #region Menu

  /// <summary>
  /// Implements menu interfaces
  /// </summary>
  public class Menu
  {
    [DllImport("user32.dll")]
    public static extern MenuItemFlags GetMenuState(
      IntPtr hMenu, int itemID, MenuItemFlags uFlag);

    [DllImport("user32.dll")]
    public static extern bool GetMenuString(
      IntPtr hMenu, int itemID, System.Text.StringBuilder lpString, int maxlen, MenuItemFlags uFlag);

    [DllImport("user32.dll")]
    public static extern int GetMenuItemInfo(IntPtr hMenu, int itemindex, bool fByPosition, ref MENUITMEINFO infos);

    [DllImport("user32.dll")]
    public static extern int GetMenuItemID( IntPtr hMenu, int index);

    [DllImport("user32.dll")]
    public static extern bool InsertMenuItem(IntPtr hMenu, int itemindex, bool fByPosition, ref MENUITMEINFO infos);

    [DllImport("user32.dll")]
    public static extern int GetMenuItemCount(IntPtr hMenu);

    [DllImport("user32.dll")]
    public static extern IntPtr CreatePopupMenu();

    [DllImport("user32.dll")]
    public static extern bool DestroyMenu(IntPtr hMenu);

    [DllImport("user32.dll")]
    public static extern bool DeleteMenu(	IntPtr hMenu,	int index, MenuItemFlags uFlag);

    [DllImport("user32.dll", SetLastError=true)]
    public static extern int TrackPopupMenu( IntPtr hmenu, TrackPopupMenuFlags fuFlags,	int x, int y, int res, IntPtr hwnd, IntPtr rect);

  } // class Menu

  #endregion

  #region Shell32

  /// <summary>
  /// Imported functions from Shell32.dll.
  /// </summary>
  public class Shell32
  {
    /// <summary>
    /// Displays a dialog box enabling the user to select a Shell folder.
    /// </summary>
    /// <param name="bi">BROWSEINFO structure that contains information used to display the dialog box.</param>
    [ DllImport( "shell32.dll", PreserveSig = true, CharSet = CharSet.Auto ) ]
    public static extern IntPtr SHBrowseForFolder(BROWSEINFO bi);
    
    /// <summary>
    /// Converts an item identifier list to a file system path. 
    /// </summary>
    /// <param name="pidl">Address of an item identifier list.</param>
    /// <param name="pszPath">Address of a buffer to receive the file system path.</param>
    //[ DllImport( "shell32.dll", PreserveSig = true, CharSet = CharSet.Auto ) ]
    //public static extern bool SHGetPathFromIDList( IntPtr pidl, IntPtr pszPath );   
  
    [ DllImport("shell32", CharSet = CharSet.Unicode) ]
    public static extern bool SHGetPathFromIDList(IntPtr pidl, IntPtr pszPath);

    [DllImport("shell32.dll", PreserveSig=true, CharSet=CharSet.Auto)]
    public static extern int SHGetSpecialFolderLocation(IntPtr hwnd, int csidl, ref IntPtr ppidl);

    [DllImport("Shell32.dll")] 
    public static extern IntPtr SHGetFileInfo( string pszPath, FILE dwFileAttributes, ref SHFILEINFO psfi, uint cbfileInfo, SHGFI uFlags ); 

  } // class Shell32

  #endregion

  #region Gdi32

  /// <summary>
  /// Imported functions from Gdi32.dll.
  /// </summary>
  public class Gdi32
  {
    [ DllImport( "gdi32.dll" ) ]
    public static extern bool DeleteObject( IntPtr hObject  );

    [DllImport("gdi32.dll", CharSet=CharSet.Auto)]
    static public extern bool TransparentBlt( IntPtr hdcDest, int nXOriginDest, int nYOriginDest, int nWidthDest, int hHeightDest,
      IntPtr hdcSrc, int nXOriginSrc, int nYOriginSrc, int nWidthSrc, int hHeightSrc,
      int crTransparent);

    [DllImport("gdi32.dll", CharSet=CharSet.Auto)]
    static public extern IntPtr GetDC(IntPtr hWnd);

    [DllImport("gdi32.dll")]
    static public extern bool StretchBlt(IntPtr hDCDest, int XOriginDest, int YOriginDest, int WidthDest, int HeightDest,
      IntPtr hDCSrc,  int XOriginScr, int YOriginSrc, int WidthScr, int HeightScr, PatBltTypes Rop);

    [DllImport("gdi32.dll")]
    static public extern IntPtr CreateCompatibleDC(IntPtr hDC);

    [DllImport("gdi32.dll")]
    static public extern IntPtr CreateCompatibleBitmap(IntPtr hDC, int Width, int Heigth);

    [DllImport("gdi32.dll")]
    static public extern IntPtr SelectObject(IntPtr hDC, IntPtr hObject);

    [DllImport("gdi32.dll")]
    static public extern bool BitBlt(IntPtr hDCDest, int XOriginDest, int YOriginDest, int WidthDest, int HeightDest,
      IntPtr hDCSrc,  int XOriginScr, int YOriginSrc, PatBltTypes flags);

    [DllImport("gdi32.dll")]
    static public extern IntPtr DeleteDC(IntPtr hDC);

    [DllImport("gdi32.dll")]
    static public extern bool PatBlt(IntPtr hDC, int XLeft, int YLeft, int Width, int Height, uint Rop);

    [DllImport("gdi32.dll")]
    static public extern uint GetPixel(IntPtr hDC, int XPos, int YPos);

    [DllImport("gdi32.dll")]
    static public extern int SetMapMode(IntPtr hDC, int fnMapMode);

    [DllImport("gdi32.dll")]
    static public extern int GetObjectType(IntPtr handle);

    [DllImport("gdi32")]
    public static extern IntPtr CreateDIBSection(IntPtr hdc, ref BITMAPINFO_FLAT bmi, 
      int iUsage, ref int ppvBits, IntPtr hSection, int dwOffset);

    [DllImport("gdi32")]
    public static extern int GetDIBits(IntPtr hDC, IntPtr hbm, int StartScan, int ScanLines, int lpBits, BITMAPINFOHEADER bmi, int usage);

    [DllImport("gdi32")]
    public static extern int GetDIBits(IntPtr hdc, IntPtr hbm, int StartScan, int ScanLines, int lpBits, ref BITMAPINFO_FLAT bmi, int usage);

    [DllImport("gdi32")]
    public static extern IntPtr GetPaletteEntries(IntPtr hpal, int iStartIndex, int nEntries, byte[] lppe);

    [DllImport("gdi32")]
    public static extern IntPtr GetSystemPaletteEntries(IntPtr hdc, int iStartIndex, int nEntries, byte[] lppe);

    [DllImport("gdi32")]
    public static extern uint SetDCBrushColor(IntPtr hdc,  uint crColor);

    [DllImport("gdi32")]
    public static extern IntPtr CreateSolidBrush(uint crColor);

    [DllImport("gdi32")]
    public static extern int SetBkMode(IntPtr hDC, BackgroundMode mode);

    [DllImport("gdi32")]
    public static extern int SetViewportOrgEx(IntPtr hdc,  int x, int y,  int param);

    [DllImport("gdi32")]
    public static extern uint SetTextColor(IntPtr hDC, uint colorRef);

    [DllImport("gdi32")]
    public static extern int SetStretchBltMode(IntPtr hDC, StrechModeFlags StrechMode);

    [DllImport("gdi32")]
    public static extern uint SetPixel(IntPtr hDC, int x, int y, uint color);

  }

  #endregion

  #region ComCtl32

  class ComCtl32
  {
    [DllImport("ComCtl32.dll")]
    public static extern bool InitCommonControlsEx( ref INITCOMMONCONTROLSEX ComCtls );

    [DllImport("comctl32.dll")] 
    public static extern int ImageList_GetImageCount( IntPtr himl );

    [DllImport("comctl32.dll")]
    public static extern IntPtr ImageList_Merge( IntPtr himl1, int i1, IntPtr himl2, int i2, int dx, int dy );

    [DllImport("comctl32.dll")]
    public static extern IntPtr ImageList_GetIcon( IntPtr himl, int i, uint flags );

    public static int GetMajorVersion()
    {
      DLLVERSIONINFO2 pdvi = new DLLVERSIONINFO2();
      pdvi.info1.cbSize = Marshal.SizeOf(typeof(DLLVERSIONINFO2));
      DllGetVersion(ref pdvi);
      return pdvi.info1.dwMajorVersion;
    }


    [DllImport("Comctl32.dll", CharSet=CharSet.Auto)]
    static private extern int DllGetVersion( ref DLLVERSIONINFO2 pdvi);

  }


  #endregion

  #region COMInterfaces

  /// <summary>
  /// Implements COM interfaces
  /// </summary>
  public class COMInterfaces
  {

    #region IShellFolder
    [ComImport,
      Guid("000214E6-0000-0000-C000-000000000046"),
      InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
      public interface IShellFolder
    {
      void ParseDisplayName(
        IntPtr hwnd, 
        IntPtr pbc, 
        [MarshalAs(UnmanagedType.LPWStr)] string pszDisplayName,
        out uint pchEaten, 
        out IntPtr ppidl, 
        ref uint pdwAttributes);

      [PreserveSig]
      int EnumObjects(IntPtr hWnd, ShellFolderEnumObjectsTypes flags,  ref IEnumIDList enumList);

      void BindToObject(
        IntPtr pidl, 
        IntPtr pbc, 
        [In()] ref Guid riid, 
        out IShellFolder ppv);
      //[MarshalAs(UnmanagedType.Interface)] out object ppv);

      void BindToStorage(
        IntPtr pidl, 
        IntPtr pbc, 
        [In()] ref Guid riid, 
        [MarshalAs(UnmanagedType.Interface)] out object ppv);

      [PreserveSig()]    
      uint CompareIDs(
        int lParam, 
        IntPtr pidl1, 
        IntPtr pidl2);

      void CreateViewObject(
        IntPtr hwndOwner, 
        [In()] ref Guid riid, 
        [MarshalAs(UnmanagedType.Interface)] out object ppv);

      void GetAttributesOf(
        uint cidl, 
        [In(), MarshalAs(UnmanagedType.LPArray)] IntPtr[] apidl,
        ref ShellFolderAttributes rgfInOut);

      [return: MarshalAs(UnmanagedType.Interface)]
      object GetUIObjectOf(
        IntPtr hwndOwner, 
        uint cidl, 
        [MarshalAs(UnmanagedType.LPArray)] IntPtr[] apidl,
        [In()] ref Guid riid, 
        IntPtr rgfReserved);

      void GetDisplayNameOf(
        IntPtr pidl, 
        ShellFolderDisplayNames uFlags, 
        out STRRET pName);

      IntPtr SetNameOf(
        IntPtr hwnd, 
        IntPtr pidl, 
        [MarshalAs(UnmanagedType.LPWStr)] string pszName, 
        ShellFolderDisplayNames uFlags);
    }
    #endregion

    #region IEnumIDList
    [ComImport(),
      Guid("000214F2-0000-0000-C000-000000000046"),
      InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
      public interface IEnumIDList
    {
      [PreserveSig()]
      uint Next(
        uint celt,
        [In(), Out(), MarshalAs(UnmanagedType.LPArray)] IntPtr[] rgelt,
        out uint pceltFetched);
        
      void Skip(
        uint celt);

      void Reset();

      IEnumIDList Clone();
    }
    #endregion

    #region IDataObject
    [ComImport(), InterfaceType(ComInterfaceType.InterfaceIsIUnknown), GuidAttribute("0000010e-0000-0000-C000-000000000046")]
      public interface IDataObject
    {
      [PreserveSig()]
      int GetData(ref FORMATETC a, ref STGMEDIUM b);
      [PreserveSig()]
      void GetDataHere(int a, ref STGMEDIUM b);
      [PreserveSig()]
      int QueryGetData(int a);
      [PreserveSig()]
      int GetCanonicalFormatEtc(int a, ref int b);
      [PreserveSig()]
      int SetData(int a, int b, int c);
      [PreserveSig()]
      int EnumFormatEtc(uint a, ref Object b);
      [PreserveSig()]
      int DAdvise(int a, uint b, Object c, ref uint d);
      [PreserveSig()]
      int DUnadvise(uint a);
      [PreserveSig()]
      int EnumDAdvise(ref Object a);
    }
    #endregion

    #region IShellExtInit
    [ComImport(), InterfaceType(ComInterfaceType.InterfaceIsIUnknown), GuidAttribute("000214e8-0000-0000-c000-000000000046")]
      public interface IShellExtInit
    {
      [PreserveSig()]
      int	Initialize (IntPtr pidlFolder, IntPtr lpdobj, uint /*HKEY*/ hKeyProgID);
    }
    #endregion

    #region IContextMenu
    [ComImport(), InterfaceType(ComInterfaceType.InterfaceIsIUnknown), GuidAttribute("000214e4-0000-0000-c000-000000000046")]
      public interface IContextMenu
    {
      // IContextMenu methods
      [PreserveSig()]
      int		QueryContextMenu(uint hmenu, uint iMenu, int idCmdFirst, int idCmdLast, uint uFlags);
      [PreserveSig()]
      void	InvokeCommand (ref CMINVOKECOMMANDINFO pici);
      [PreserveSig()]
      void	GetCommandString(
        int idcmd,
        GetCommandStringInformations uflags,
        int reserved,
        StringBuilder commandstring,
        int cch);
    }
    #endregion

    #region IContextMenu2
    [ComImport(), InterfaceType(ComInterfaceType.InterfaceIsIUnknown), GuidAttribute("000214f4-0000-0000-c000-000000000046")]
      public interface IContextMenu2
    {
      // IContextMenu methods
      [PreserveSig()]
      int		QueryContextMenu(uint hmenu, uint iMenu, int idCmdFirst, int idCmdLast, uint uFlags);
      [PreserveSig()]
      void	InvokeCommand (ref CMINVOKECOMMANDINFO pici);
      [PreserveSig()]
      void	GetCommandString(int idcmd,
        GetCommandStringInformations uflags,
        int reserved,
        StringBuilder commandstring,
        int cch);
      // IContextMenu2 methods
      [PreserveSig()]
      uint HandleMenuMsg(uint uMsg,IntPtr wParam,IntPtr lParam);
    }
    #endregion

  } // class COMInterfaces

  #endregion

  #region ColorUtils
  /// <summary>
  /// Implements color utilities.
  /// </summary>
  public class ColorUtil
  {
		
    #region fields

    static Color backgroundColor = Color.Empty;
    static Color selectionColor = Color.Empty;
    static Color selectionUnfocusedColor = Color.Empty;
    static Color controlColor = Color.Empty;
    static Color pressedColor = Color.Empty;
    static Color checkedColor = Color.Empty;
    static Color borderColor = Color.Empty;
    static bool useCustomColor = false;

    #endregion
		
    #region Constructor
    // No need to construct this object
    private ColorUtil()
    {
    }
    #endregion

    #region properties

    static public bool UsingCustomColor
    {
      get { return useCustomColor;}
    }

    #endregion

    #region knowncolor names

    static public string[] KnownColorNames = 
    {
        "Transparent", "Black", "DimGray", "Gray", "DarkGray", "Silver", "LightGray", "Gainsboro", "WhiteSmoke", "White",
      "RosyBrown", "IndianRed", "Brown", "Firebrick", "LightCoral", "Maroon", "DarkRed", "Red", "Snow", "MistyRose",
      "Salmon", "Tomato", "DarkSalmon", "Coral", "OrangeRed", "LightSalmon", "Sienna", "SeaShell", "Chocalate",
      "SaddleBrown", "SandyBrown", "PeachPuff", "Peru", "Linen", "Bisque", "DarkOrange", "BurlyWood", "Tan", "AntiqueWhite",
      "NavajoWhite", "BlanchedAlmond", "PapayaWhip", "Mocassin", "Orange", "Wheat", "OldLace", "FloralWhite", "DarkGoldenrod",
      "Cornsilk", "Gold", "Khaki", "LemonChiffon", "PaleGoldenrod", "DarkKhaki", "Beige", "LightGoldenrod", "Olive",
      "Yellow", "LightYellow", "Ivory", "OliveDrab", "YellowGreen", "DarkOliveGreen", "GreenYellow", "Chartreuse", "LawnGreen",
      "DarkSeaGreen", "ForestGreen", "LimeGreen", "PaleGreen", "DarkGreen", "Green", "Lime", "Honeydew", "SeaGreen", "MediumSeaGreen",
      "SpringGreen", "MintCream", "MediumSpringGreen", "MediumAquaMarine", "YellowAquaMarine", "Turquoise", "LightSeaGreen",
      "MediumTurquoise", "DarkSlateGray", "PaleTurquoise", "Teal", "DarkCyan", "Aqua", "Cyan", "LightCyan", "Azure", "DarkTurquoise",
      "CadetBlue", "PowderBlue", "LightBlue", "DeepSkyBlue", "SkyBlue", "LightSkyBlue", "SteelBlue", "AliceBlue", "DodgerBlue",
      "SlateGray", "LightSlateGray", "LightSteelBlue", "CornflowerBlue", "RoyalBlue", "MidnightBlue", "Lavender", "Navy",
      "DarkBlue", "MediumBlue", "Blue", "GhostWhite", "SlateBlue", "DarkSlateBlue", "MediumSlateBlue", "MediumPurple",
      "BlueViolet", "Indigo", "DarkOrchid", "DarkViolet", "MediumOrchid", "Thistle", "Plum", "Violet", "Purple", "DarkMagenta",
      "Magenta", "Fuchsia", "Orchid", "MediumVioletRed", "DeepPink", "HotPink", "LavenderBlush", "PaleVioletRed", "Crimson",
      "Pink", "LightPink" };

    #endregion
		
    #region Systemcolors names
    static public string[] SystemColorNames = 
    {
      "ActiveBorder", "ActiveCaption", "ActiveCaptionText", "AppWorkspace", "Control", "ControlDark", "ControlDarkDark",
      "ControlLight", "ControlLightLight", "ControlText", "Desktop", "GrayText", "HighLight", "HighLightText", 
      "HotTrack", "InactiveBorder", "InactiveCaption", "InactiveCaptionText", "Info", "InfoText", "Menu", "MenuText",
      "ScrollBar", "Window", "WindowFrame", "WindowText" };
    #endregion
		
    #region Conversion between RGB and Hue, Saturation and Luminosity function helpers
    static public void HSLToRGB(float h, float s, float l, ref float r, ref float g, ref float b)
    {
      // given h,s,l,[240 and r,g,b [0-255]
      // convert h [0-360], s,l,r,g,b [0-1]
      h=(h/240)*360;
      s /= 240;
      l /= 240;
      r /= 255;
      g /= 255;
      b /= 255;
					
      // Begin Foley
      float m1,m2;
			
      // Calc m2
      if (l<=0.5f) 
      {
        //m2=(l*(l+s)); seems to be typo in Foley??, replace l for 1
        m2=(l*(1+s));				
      } 
      else 
      {
        m2=(l+s-l*s);
      }
			
      //calc m1
      m1=2.0f*l-m2;
			
      //calc r,g,b in [0-1]
      if (s==0.0f) 
      {	// Achromatic: There is no hue
        // leave out the UNDEFINED part, h will always have value
        r=g=b=l;
      } 
      else 
      {	// Chromatic: There is a hue
        r= getRGBValue(m1,m2,h+120.0f);
        g= getRGBValue(m1,m2,h);
        b= getRGBValue(m1,m2,h-120.0f);
      }
						
      // End Foley
      // convert to 0-255 ranges
      r*=255;
      g*=255;
      b*=255;
            
    }

    static private float getRGBValue(float n1, float n2, float hue)
    {
      // Helper function for the HSLToRGB function above
      if (hue>360.0f) 
      {
        hue-=360.0f;
      } 
      else if (hue<0.0f) 
      {
        hue+=360.0f;
      }
		
      if (hue<60.0) 
      {
        return n1+(n2-n1)*hue/60.0f;
      } 
      else if (hue<180.0f) 
      {
        return n2;
      } 
      else if (hue<240.0f) 
      {
        return n1+(n2-n1)*(240.0f-hue)/60.0f;
      } 
      else 
      {
        return n1;
      }
    }

    static public void RGBToHSL(int r, int g, int b, ref float h, ref float s, ref float l)
    {

      float delta;
      float fr = (float)r/255;
      float fg = (float)g/255;
      float fb = (float)b/255;
      float max = Math.Max(fr,Math.Max(fg,fb));
      float min = Math.Min(fr,Math.Min(fg,fb));
			
      //calc the lightness
      l = (max+min)/2;
			
      if (max==min)
      {
        //should be undefined but this works for what we need
        s = 0;
        h = 240.0f;		
      } 
      else 
      {
			
        delta = max-min;						
			
        //calc the Saturation
        if (l < 0.5) 
        {
          s = delta/(max+min);
        } 
        else 
        {
          s = delta/(2.0f-(max+min));
        }
				
        //calc the hue
        if (fr==max) 
        {
          h = (fg-fb)/delta;
        } 
        else if (fg==max) 
        {
          h = 2.0f + (fb-fr)/delta;
        } 
        else if (fb==max) 
        {
          h = 4.0f + (fr-fg)/delta;
        }
				
        //convert hue to degrees
        h*=60.0f;
        if (h<0.0f) 
        {
          h+=360.0f;
        }
      }
		
      //convert to 0-255 ranges
      //h [0-360], h,l [0-1]
      l*=240;
      s*=240;
      h=(h/360)*240;
            
    }
    #endregion

    #region Visual Studio .NET colors calculation helpers
       
    static public Color VSNetBackgroundColor
    {
      get 
      {
        if ( useCustomColor && backgroundColor != Color.Empty )
          return backgroundColor;
        else
          return CalculateColor(SystemColors.Window, SystemColors.Control, 220);
      }
      set
      {
        // Flag that we are going to use custom colors instead
        // of calculating the color based on the system colors
        // -- this is a way of hooking up into the VSNetColors that I use throughout
        // the UtilityLibrary
        useCustomColor = true;
        backgroundColor = value;
      }
    }

    static public Color VSNetSelectionUnfocusedColor
    {
      get 
      {
        if ( useCustomColor && selectionColor != Color.Empty )
          return selectionUnfocusedColor;
        else
          return CalculateColor(SystemColors.Highlight, SystemColors.Window, 25);
      }
      set
      {
        // Flag that we are going to use custom colors instead
        // of calculating the color based on the system colors
        // -- this is a way of hooking up into the VSNetColor that I use throughout
        // the UtilityLibrary
        useCustomColor = true;
        selectionUnfocusedColor = value;
      }
    }

    static public Color VSNetSelectionColor
    {
      get 
      {
        if ( useCustomColor && selectionColor != Color.Empty )
          return selectionColor;
        else
          return CalculateColor(SystemColors.Highlight, SystemColors.Window, 70);
      }
      set
      {
        // Flag that we are going to use custom colors instead
        // of calculating the color based on the system colors
        // -- this is a way of hooking up into the VSNetColor that I use throughout
        // the UtilityLibrary
        useCustomColor = true;
        selectionColor = value;
      }
    }


    static public Color VSNetControlColor
    {
      get 
      {
          if ( useCustomColor && controlColor != Color.Empty )
          return controlColor;
        else
          return CalculateColor(SystemColors.Control, VSNetBackgroundColor, 195);
      }
      set
      {
        // Flag that we are going to use custom colors instead
        // of calculating the color based on the system colors
        // -- this is a way of hooking up into the VSNetColors that I use throughout
        // the UtilityLibrary
        useCustomColor = true;
        controlColor = value;
      }

    }

    static public Color VSNetPressedColor
    {
      get 
      {
        if ( useCustomColor && pressedColor != Color.Empty )
          return pressedColor;
        else
          return CalculateColor(SystemColors.Highlight, ColorUtil.VSNetSelectionColor, 70);
      }
      set
      {
        // Flag that we are going to use custom colors instead
        // of calculating the color based on the system colors
        // -- this is a way of hooking up into the VSNetColors that I use throughout
        // the UtilityLibrary
        useCustomColor = true;
        pressedColor = value;
      }
    }


    static public Color VSNetCheckedColor
    {
      get 
      {
        if ( useCustomColor && pressedColor != Color.Empty )
          return checkedColor;
        else
          return CalculateColor(SystemColors.Highlight,  SystemColors.Window, 30);
      }
      set
      {
        // Flag that we are going to use custom colors instead
        // of calculating the color based on the system colors
        // -- this is a way of hooking up into the VSNetColors that I use throughout
        // the UtilityLibrary
        useCustomColor = true;
        checkedColor = value;
      }
    }

    static public Color VSNetBorderColor
    {
      get 
      {
        if ( useCustomColor && borderColor != Color.Empty )
          return borderColor;
        else
        {
          // This color is the default color unless we are using 
          // custom colors
          return SystemColors.Highlight;
        }
      }
      set
      {
        // Flag that we are going to use custom colors instead
        // of calculating the color based on the system colors
        // -- this is a way of hooking up into the VSNetColors that I use throughout
        // the UtilityLibrary
        useCustomColor = true;
        borderColor = value;
      }
    }

    public static Color CalculateColor(Color front, Color back, int alpha)
    {
			
      // Use alpha blending to brigthen the colors but don't use it
      // directly. Instead derive an opaque color that we can use.
      // -- if we use a color with alpha blending directly we won't be able 
      // to paint over whatever color was in the background and there
      // would be shadows of that color showing through
      Color frontColor = Color.FromArgb(255, front);
      Color backColor = Color.FromArgb(255, back);
									
      float frontRed = frontColor.R;
      float frontGreen = frontColor.G;
      float frontBlue = frontColor.B;
      float backRed = backColor.R;
      float backGreen = backColor.G;
      float backBlue = backColor.B;
			
      float fRed = frontRed*alpha/255 + backRed*((float)(255-alpha)/255);
      byte newRed = (byte)fRed;
      float fGreen = frontGreen*alpha/255 + backGreen*((float)(255-alpha)/255);
      byte newGreen = (byte)fGreen;
      float fBlue = frontBlue*alpha/255 + backBlue*((float)(255-alpha)/255);
      byte newBlue = (byte)fBlue;

      return  Color.FromArgb(255, newRed, newGreen, newBlue);

    }
    #endregion

    #region General functions

    static public Color ColorFromPoint(Graphics g, int x, int y)
    {
      IntPtr hDC = g.GetHdc();
      // Get the color of the pixel first
      uint colorref = Gdi32.GetPixel(hDC, x, y);
      byte Red = GetRValue(colorref);
      byte Green = GetGValue(colorref);
      byte Blue = GetBValue(colorref);
      g.ReleaseHdc(hDC);
      return  Color.FromArgb(Red, Green, Blue);
    }

    static public bool IsKnownColor(Color color, ref Color knownColor, bool useTransparent)
    {

      // Using the Color structrure "FromKnowColor" does not work if 
      // we did not create the color as a known color to begin with
      // we need to compare the rgbs of both color 
      Color currentColor = Color.Empty;
      bool badColor = false;
      for (KnownColor enumValue = 0; enumValue <= KnownColor.YellowGreen; enumValue++)
      {
        currentColor = Color.FromKnownColor(enumValue);
        string colorName = currentColor.Name;
        if ( !useTransparent ) 
          badColor = (colorName == "Transparent");
        if ( color.A == currentColor.A && color.R == currentColor.R && color.G == currentColor.G 
          && color.B == currentColor.B && !currentColor.IsSystemColor
          && !badColor )
        {
          knownColor = currentColor;
          return true;
        }
				
      }
      return false;

    }

    static public bool IsSystemColor(Color color, ref Color knownColor)
    {

      // Using the Color structrure "FromKnowColor" does not work if 
      // we did not create the color as a known color to begin with
      // we need to compare the rgbs of both color 
      Color currentColor = Color.Empty;
      for (KnownColor enumValue = 0; enumValue <= KnownColor.YellowGreen; enumValue++)
      {
        currentColor = Color.FromKnownColor(enumValue);
        string colorName = currentColor.Name;
        if ( color.R == currentColor.R && color.G == currentColor.G 
          && color.B == currentColor.B && currentColor.IsSystemColor )
        {
          knownColor = currentColor;
          return true;
        }
				
      }
      return false;
    }

    static public uint GetCOLORREF(Color color)
    {
      return RGB(color.R, color.G, color.B);
    }

    static public Color ColorFromRGBString(string text)
    {
			
      Color rgbColor = Color.Empty;
      string[] RGBs = text.Split(','); 
      if ( RGBs.Length != 3 ) 
      {
        // If we don't have three pieces of information, then the
        // string is not properly formatted, inform the use
        throw new Exception("RGB color string is not well formed");
      }
		
      string stringR = RGBs[0];
      string stringG = RGBs[1];
      string stringB = RGBs[2];
      int R, G, B;
				
      try 
      {
        R = Convert.ToInt32(stringR);
        G = Convert.ToInt32(stringG);
        B = Convert.ToInt32(stringB);
        if ( ( R < 0 || R > 255 ) || ( G < 0 || G > 255 ) || ( B < 0 || B > 255 ) ) 
        {
          throw new Exception("Out of bounds RGB value");
        }
        else 
        {
          // Convert to color 
          rgbColor = Color.FromArgb(R, G, B);
          // See if we have either a web color or a systgem color
          Color knownColor = Color.Empty;
          bool isKnown = ColorUtil.IsKnownColor( rgbColor, ref knownColor, true);
          if ( !isKnown )
            isKnown = ColorUtil.IsSystemColor(rgbColor, ref knownColor);
          if ( isKnown )
            rgbColor = knownColor;
        }
      }
      catch ( InvalidCastException )
      {
        throw new Exception("Invalid RGB value");
      }

      return rgbColor;
    }
    static public Color LightColor(Color color, int inc)
    {
      int red = color.R;
      int green = color.G;
      int blue = color.B;

      if ( red + inc <= 255 )
        red += inc;
      if ( green + inc <= 255 )
        green += inc;
      if ( blue + inc <= 255 )
        blue += inc;

      return Color.FromArgb(red, green, blue);
    }
    static public Color DarkColor(Color color, int inc)
    {
      int red = color.R;
      int green = color.G;
      int blue = color.B;

      if ( red >= inc )
        red -= inc;
      if ( green >= inc )
        green -= inc;
      if ( blue >= inc )
        blue -= inc;

      return Color.FromArgb(red, green, blue);
			
    }
    #endregion
		
    #region Windows RGB related macros
    static public byte GetRValue(uint color)
    {
      return (byte)color;
    }

    static public byte GetGValue(uint color)
    {
      return ((byte)(((short)(color)) >> 8));
    }

    static public byte GetBValue(uint color)
    {
      return ((byte)((color)>>16));
    }

    static public uint RGB(int r, int g, int b)
    {
      return ((uint)(((byte)(r)|((short)((byte)(g))<<8))|(((short)(byte)(b))<<16)));

    }
    #endregion

  }


  #endregion

  #region TextUtils
  /// <summary>
  /// Implements text utilities.
  /// </summary>
  public class TestUtil
  {
    #region constructor

    // No need to construct this object
    private TestUtil()
    {
    }

    #endregion

    #region methods

    /// <summary>
    /// Gets the text size using the graphics and font
    /// </summary>
    /// <param name="graphics"></param>
    /// <param name="text"></param>
    /// <param name="font"></param>
    /// <returns></returns>
    public static Size GetTextSize(Graphics graphics, string text, Font font)
    {
      IntPtr hdc = IntPtr.Zero;
      if ( graphics != null )
      {
        // Get device context from the graphics passed in
        hdc = graphics.GetHdc();
      }
      else
      {
        // Get screen device context
        hdc = Gdi32.GetDC(IntPtr.Zero);
      }

      IntPtr fontHandle = font.ToHfont();
      IntPtr currentFontHandle = Gdi32.SelectObject(hdc, fontHandle);
			
      RECT rect = new RECT();
      rect.left = 0;
      rect.right = 0;
      rect.top = 0;
      rect.bottom = 0;
		
      User32.DrawText(hdc, text, text.Length, ref rect, DrawTextFormatFlags.DT_SINGLELINE | DrawTextFormatFlags.DT_LEFT | DrawTextFormatFlags.DT_CALCRECT);
      Gdi32.SelectObject(hdc, currentFontHandle);
      Gdi32.DeleteObject(fontHandle);

      if(graphics != null)
        graphics.ReleaseHdc(hdc);
      else
        User32.ReleaseDC(IntPtr.Zero, hdc);
							
      return new Size(rect.right - rect.left, rect.bottom - rect.top);
    } // GetTextSize()

    /// <summary>
    /// Gets the text size
    /// </summary>
    /// <param name="graphics"></param>
    /// <param name="text"></param>
    /// <param name="font"></param>
    /// <param name="rc"></param>
    /// <param name="drawFlags"></param>
    /// <returns></returns>
    public static Size GetTextSize(Graphics graphics, string text, Font font, ref Rectangle rc, DrawTextFormatFlags drawFlags)
    {
      IntPtr hdc = IntPtr.Zero;
      if ( graphics != null )
      {
        // Get device context from the graphics passed in
        hdc = graphics.GetHdc();
      }
      else
      {
        // Get screen device context
        hdc = Gdi32.GetDC(IntPtr.Zero);
      }

      IntPtr fontHandle = font.ToHfont();
      IntPtr currentFontHandle = Gdi32.SelectObject(hdc, fontHandle);
			
      RECT rect = new RECT();
      rect.left = rc.Left;
      rect.right = rc.Right;
      rect.top = rc.Top;
      rect.bottom = rc.Bottom;
		
      User32.DrawText(hdc, text, text.Length, ref rect, drawFlags);
      Gdi32.SelectObject(hdc, currentFontHandle);
      Gdi32.DeleteObject(fontHandle);

      if(graphics != null)
        graphics.ReleaseHdc(hdc);
      else
        User32.ReleaseDC(IntPtr.Zero, hdc);
							
      return new Size(rect.right - rect.left, rect.bottom - rect.top);
			
    } // GetTextSize()

    /// <summary>
    /// Draw text
    /// </summary>
    /// <param name="graphics"></param>
    /// <param name="text"></param>
    /// <param name="font"></param>
    /// <param name="rect"></param>
    public static void DrawText(Graphics graphics, string text, Font font, Rectangle rect)
    {
      IntPtr hdc = graphics.GetHdc();
      IntPtr fontHandle = font.ToHfont();
      IntPtr currentFontHandle = Gdi32.SelectObject(hdc, fontHandle);
      Gdi32.SetBkMode(hdc, BackgroundMode.TRANSPARENT);
           						
      RECT rc = new RECT();
      rc.left = rect.Left;
      rc.top = rect.Top;
      rc.right = rc.left + rect.Width;
      rc.bottom = rc.top + rect.Height;
			
      User32.DrawText(hdc, text, text.Length, ref rc, 
                      DrawTextFormatFlags.DT_SINGLELINE | DrawTextFormatFlags.DT_LEFT | 
                      DrawTextFormatFlags.DT_MODIFYSTRING | DrawTextFormatFlags.DT_WORD_ELLIPSIS);

      Gdi32.SelectObject(hdc, currentFontHandle);
      Gdi32.DeleteObject(fontHandle);
      graphics.ReleaseHdc(hdc);
    } // DrawText
  
    /// <summary>
    /// Draw text
    /// </summary>
    /// <param name="graphics"></param>
    /// <param name="text"></param>
    /// <param name="font"></param>
    /// <param name="rect"></param>
    /// <param name="textColor"></param>
    public static void DrawText(Graphics graphics, string text, Font font, Rectangle rect, Color textColor)
    {
      IntPtr hdc = graphics.GetHdc();
      IntPtr fontHandle = font.ToHfont();
      IntPtr currentFontHandle = Gdi32.SelectObject(hdc, fontHandle);
      uint colorRef = ColorUtil.GetCOLORREF(textColor);
      Gdi32.SetTextColor(hdc, colorRef);
      Gdi32.SetBkMode(hdc, BackgroundMode.TRANSPARENT);
           						
      RECT rc = new RECT();
      rc.left = rect.Left;
      rc.top = rect.Top;
      rc.right = rc.left + rect.Width;
      rc.bottom = rc.top + rect.Height;
			
      User32.DrawText(hdc, text, text.Length, ref rc, 
                      DrawTextFormatFlags.DT_SINGLELINE | DrawTextFormatFlags.DT_LEFT | 
                      DrawTextFormatFlags.DT_MODIFYSTRING| DrawTextFormatFlags.DT_WORD_ELLIPSIS);

      Gdi32.SelectObject(hdc, currentFontHandle);
      Gdi32.DeleteObject(fontHandle);
      graphics.ReleaseHdc(hdc);
    } // DrawText()

    /// <summary>
    /// Draw reverse string
    /// </summary>
    /// <param name="g"></param>
    /// <param name="drawText"></param>
    /// <param name="drawFont"></param>
    /// <param name="drawRect"></param>
    /// <param name="drawBrush"></param>
    /// <param name="drawFormat"></param>
    public static void DrawReverseString(Graphics g, 	String drawText, 	Font drawFont, 	Rectangle drawRect, 	Brush drawBrush, StringFormat drawFormat )
    {
      GraphicsContainer container = g.BeginContainer();

      // The text will be rotated around the origin (0,0) and so needs moving
      // back into position by using a transform
      g.TranslateTransform(drawRect.Left * 2 + drawRect.Width, 
        drawRect.Top * 2 + drawRect.Height);

      // Rotate the text by 180 degress to reverse the direction 
      g.RotateTransform(180);

      // Draw the string as normal and let then transforms do the work
      g.DrawString(drawText, drawFont, drawBrush, drawRect, drawFormat);

      g.EndContainer(container);
    } // DrawReverseString()

    #endregion
  } // class TestUtil

  #endregion

  #region Clipboard

  /// <summary>
  /// Summary description for Clipboard.
  /// </summary>
  public class Clipboard
  {
    public static bool ClearClipboard(IntPtr hWnd)
    {
      if(!OpenClipboard(hWnd)) return false;
      bool res = EmptyClipboard();
      CloseClipboard();
      return res;
    }

    [DllImport("user32.dll")]
    protected static extern bool EmptyClipboard();

    [DllImport("user32.dll")]
    protected static extern bool OpenClipboard(IntPtr hWnd);

    [DllImport("user32.dll")]
    protected static extern bool CloseClipboard();

  } // Clipboard

  #endregion

  #region Kernel32

  public class Kernel32
  {
    [DllImport("kernel32.dll")]
    public static extern int GetLastError();


  }

  #endregion

  #region themes

  class Themes
  {
    public class Parts
    {
      public enum Window
      {
        WP_CAPTION = 1,
        WP_SMALLCAPTION = 2,
        WP_MINCAPTION = 3,
        WP_SMALLMINCAPTION = 4,
        WP_MAXCAPTION = 5,
        WP_SMALLMAXCAPTION = 6,
        WP_FRAMELEFT = 7,
        WP_FRAMERIGHT = 8,
        WP_FRAMEBOTTOM = 9,
        WP_SMALLFRAMELEFT = 10,
        WP_SMALLFRAMERIGHT = 11,
        WP_SMALLFRAMEBOTTOM = 12,
        WP_SYSBUTTON = 13, 
        WP_MDISYSBUTTON =14,
        WP_MINBUTTON = 15,
        WP_MDIMINBUTTON = 16,
        WP_MAXBUTTON = 17,
        WP_CLOSEBUTTON = 18,
        WP_SMALLCLOSEBUTTON = 19,
        WP_MDICLOSEBUTTON = 20,
        WP_RESTOREBUTTON = 21,
        WP_MDIRESTOREBUTTON = 22,
        WP_HELPBUTTON = 23,
        WP_MDIHELPBUTTON = 24,
        WP_HORZSCROLL = 25,
        WP_HORZTHUMB = 26,
        WP_VERTSCROL = 27,
        WP_VERTTHUMB = 28,
        WP_DIALOG = 29,
        WP_CAPTIONSIZINGTEMPLATE = 30,
        WP_SMALLCAPTIONSIZINGTEMPLATE = 31,
        WP_FRAMELEFTSIZINGTEMPLATE = 32,
        WP_SMALLFRAMELEFTSIZINGTEMPLATE = 33,
        WP_FRAMERIGHTSIZINGTEMPLATE = 34,
        WP_SMALLFRAMERIGHTSIZINGTEMPLATE = 35,
        WP_FRAMEBOTTOMSIZINGTEMPLATE = 36,
        WP_SMALLFRAMEBOTTOMSIZINGTEMPLATE = 37
      }
    }


    public class PartStates
    {
      public enum MINBUTTON
      {
        MINBS_NORMAL = 1,
        MINBS_HOT = 2,
        MINBS_PUSHED = 3,
        MINBS_DISABLED = 4,
      }

      public enum MAXBUTTON
      {
        MAXBS_NORMAL = 1,
        MAXBS_HOT = 2,
        MAXBS_PUSHED = 3,
        MAXBS_DISABLED = 4,
      }

      public enum RESTOREBUTTON
      {
        RBS_NORMAL = 1,
        RBS_HOT = 2,
        RBS_PUSHED = 3,
        RBS_DISABLED = 4,
      }
    }
    
    public static bool Supported()
    {
      // check if themes are supported on the current OS
      Version version = Environment.OSVersion.Version;
      Version targetVersion = new Version("5.1.2600.0");
      return ((version >= targetVersion) && (Environment.OSVersion.Platform == PlatformID.Win32NT ));
    }
    
   
    /// <summary>
    /// Tests if a visual style for the current application is active.
    /// </summary>
    [DllImport("uxtheme.dll")]
    public static extern int IsThemeActive();
  		
    /// <summary>
    /// Opens the theme data for a window and its associated class.
    /// </summary>
    [DllImport("uxtheme.dll")]
    public static extern IntPtr OpenThemeData(IntPtr hWnd, [MarshalAs(UnmanagedType.LPTStr)] string classList);
  		
    /// <summary>Closes the theme data handle.</summary>
    /// <remarks>The CloseThemeData function should be called when a window that has a visual style applied is destroyed.</remarks> 
    [DllImport("uxtheme.dll")]
    public static extern void CloseThemeData(IntPtr hTheme);

    [DllImport("uxtheme.dll")]
    public static extern int IsThemeBackgroundPartiallyTransparent( IntPtr hTheme, int iPartId, int iStateId );


  		
    /// <summary>
    /// Draws the background image defined by the visual style for the specified control part.
    /// </summary>
    [DllImport("uxtheme.dll")]
    public static extern void DrawThemeBackground(IntPtr hTheme, IntPtr hDC, int partId, int stateId, ref RECT rect, ref RECT clipRect);
  		
    /// <summary>
    /// Draws one or more edges defined by the visual style of a rectangle.
    /// </summary>
    [DllImport("uxtheme.dll")]
    public static extern void DrawThemeEdge(IntPtr hTheme, IntPtr hDC, int partId, int stateId, ref RECT destRect, uint edge, uint flags, ref RECT contentRect);
  		
    /// <summary>
    /// Draws an image from an image list with the icon effect defined by the visual style.
    /// </summary>
    [DllImport("uxtheme.dll")]
    public static extern void DrawThemeIcon(IntPtr hTheme, IntPtr hDC, int partId, int stateId, ref RECT rect, IntPtr hIml, int imageIndex);
  		
    /// <summary>
    /// Draws text using the color and font defined by the visual style.
    /// </summary>
    [DllImport("uxtheme.dll")]
    public static extern void DrawThemeText(IntPtr hTheme, IntPtr hDC, int partId, int stateId, [MarshalAs(UnmanagedType.LPTStr)] string text, int charCount, uint textFlags, uint textFlags2, ref RECT rect);
  		
    /// <summary>
    /// Draws the part of a parent control that is covered by a partially-transparent or alpha-blended child control.
    /// </summary>
    [DllImport("uxtheme.dll")]
    public static extern void DrawThemeParentBackground(IntPtr hWnd, IntPtr hDC, ref RECT rect);
  		
    /// <summary>
    /// Causes a window to use a different set of visual style information than its class normally uses.
    /// </summary>
    [DllImport("uxtheme.dll")]
    public static extern void SetWindowTheme(IntPtr hWnd, string subAppName, string subIdList);
  }


  #endregion

  #region Shlwapi

  public class Shlwapi
  {
    #region PathCompactPath
    /// <summary>
    /// Truncates a file path to fit within a given pixel width by replacing path components with ellipses.
    /// </summary>
    /// <param name="hDC"></param>
    /// <param name="lpszPath"></param>
    /// <param name="dx"></param>
    /// <returns>Returns TRUE if the path was successfully compacted to the specified width. Returns FALSE on failure, or if the base portion of the path would not fit the specified width</returns>
    [DllImport("shlwapi.dll", CharSet=CharSet.Auto)]
    static public extern bool PathCompactPath(
      IntPtr hDC,
      System.Text.StringBuilder lpszPath,
      int dx);
    #endregion

    #region PathCompactPathEx
    /// <summary>
    /// Truncates a path to fit within a certain number of characters by replacing path components with ellipses
    /// </summary>
    /// <param name="pszOut"></param>
    /// <param name="pszSrc"></param>
    /// <param name="cchMax"></param>
    /// <param name="dwFlags"></param>
    /// <returns>Returns TRUE if successful, or FALSE otherwise</returns>
    [DllImport("shlwapi.dll", CharSet=CharSet.Auto)]
    static public extern bool PathCompactPathEx(
      System.Text.StringBuilder pszOut,
      System.Text.StringBuilder pszSrc,
      int cchMax,
      int dwFlags);
    #endregion

		
    #region PathIsFileSpec
    /// <summary>
    /// Searches a path for any path delimiting characters (for example, ':' or '\' ). If there are no path delimiting characters present, the path is considered to be a File Spec path
    /// </summary>
    /// <param name="path">Path searched</param>
    /// <returns>Returns TRUE if there are no path delimiting characters within the path, or FALSE if there are path delimiting characters</returns>
    [DllImport("shlwapi.dll", CharSet=CharSet.Auto)]
    static public extern bool PathIsFileSpec(
      string path);
    #endregion

    #region PathIsPrefix
    /// <summary>
    /// Searches a path to determine if it contains a valid prefix of the type passed by pszPrefix. A prefix is one of these types: "C:\\", ".", "..", "..\\"
    /// </summary>
    /// <param name="prefix"></param>
    /// <param name="path"></param>
    /// <returns>Returns TRUE if the compared path is the full prefix for the path, or FALSE otherwise</returns>
    [DllImport("shlwapi.dll", CharSet=CharSet.Auto)]
    static public extern bool PathIsPrefix(
      string prefix,
      string path);
    #endregion

    #region PathIsRelative
    /// <summary>
    /// Searches a path and determines if it is relative
    /// </summary>
    /// <param name="path"></param>
    /// <returns>Returns TRUE if the path is relative, or FALSE if it is absolute</returns>
    [DllImport("shlwapi.dll", CharSet=CharSet.Auto)]
    static public extern bool PathIsRelative(
      string path);
    #endregion

    #region PathIsRoot
    /// <summary>
    /// Parses a path to determine if it is a directory root
    /// </summary>
    /// <param name="path"></param>
    /// <returns>Returns TRUE if the specified path is a root, or FALSE otherwise</returns>
    [DllImport("shlwapi.dll", CharSet=CharSet.Auto)]
    static public extern bool PathIsRoot(
      string path);
    #endregion

    #region PathIsSameRoot

    /// <summary>
    /// Compares two paths to determine if they have a common root component
    /// </summary>
    /// <param name="path1"></param>
    /// <param name="path2"></param>
    /// <returns>Returns TRUE if both strings have the same root component, or FALSE otherwise</returns>
    [DllImport("shlwapi.dll", CharSet=CharSet.Auto)]
    static public extern bool PathIsSameRoot(string path1,string path2);

    #endregion

    #region PathIsUNC
    /// <summary>
    /// Determines if the string is a valid UNC (universal naming convention) for a server and share path
    /// </summary>
    /// <param name="path"></param>
    /// <returns>Returns TRUE if the string is a valid UNC path, or FALSE otherwise</returns>
    [DllImport("shlwapi.dll", CharSet=CharSet.Auto)]
    static public extern bool PathIsUNC(string path);
    
    #endregion

    #region PathIsUNCServer
    /// <summary>
    /// Determines if a string is a valid UNC (universal naming convention) for a server path only
    /// </summary>
    /// <param name="path"></param>
    /// <returns>Returns TRUE if the string is a valid UNC path for a server only (no share name), or FALSE otherwise</returns>
    [DllImport("shlwapi.dll", CharSet=CharSet.Auto)]
    static public extern bool PathIsUNCServer(
      string path);
    #endregion

    #region PathIsUNCServerShare
    /// <summary>
    /// Determines if a string is a valid universal naming convention (UNC) share path, \\server\share
    /// </summary>
    /// <param name="path"></param>
    /// <returns>Returns TRUE if the string is in the form \\server\share, or FALSE otherwise</returns>
    [DllImport("shlwapi.dll", CharSet=CharSet.Auto)]
    static public extern bool PathIsUNCServerShare(
      string path);
    #endregion

    #region PathIsURL
    /// <summary>
    /// Tests a given string to determine if it conforms to a valid URL format
    /// </summary>
    /// <param name="path"></param>
    /// <returns>Returns TRUE if pszPath has a valid URL format, or FALSE otherwise</returns>
    [DllImport("shlwapi.dll", CharSet=CharSet.Auto)]
    static public extern bool PathIsURL(
      string path);
    #endregion

    #region PathMakePretty
    /// <summary>
    /// Converts a path to all lowercase characters to give the path a consistent appearance
    /// </summary>
    /// <param name="path"></param>
    /// <returns>Returns TRUE if the path has been converted, or FALSE otherwise</returns>
    [DllImport("shlwapi.dll", CharSet=CharSet.Auto)]
    static public extern bool PathMakePretty(
      System.Text.StringBuilder path);
    #endregion

    #region PathMatchSpec
    /// <summary>
    /// Searches a string using a DOS wild card match type
    /// </summary>
    /// <param name="fileparam">Contains the path to be searched</param>
    /// <param name="spec">Contains the file type for which to search. For example, to test whether or not pszFileParam is a DOC file, pszSpec should be set to "*.doc"</param>
    /// <returns>Returns TRUE if the string matches, or FALSE otherwise</returns>
    [DllImport("shlwapi.dll", CharSet=CharSet.Auto)]
    static public extern bool PathMatchSpec(
      string fileparam,
      string spec);
    #endregion

    #region PathParseIconLocation
    /// <summary>
    /// Parses a file location string containing a file location and icon index, and returns separate values
    /// </summary>
    /// <param name="IconFile">Contains a file location string. It should be in the form "pathname,iconindex". When the function returns, pszIconFile will point to the file's pathname</param>
    /// <returns>Returns the valid icon index value</returns>
    [DllImport("shlwapi.dll", CharSet=CharSet.Auto)]
    static public extern int PathParseIconLocation(
      System.Text.StringBuilder IconFile);
    #endregion

    #region PathQuoteSpaces
    /// <summary>
    /// Searches a path for spaces. If spaces are found, the entire path is enclosed in quotation marks
    /// </summary>
    /// <param name="lpsz">Path to search</param>
    [DllImport("shlwapi.dll", CharSet=CharSet.Auto)]
    static public extern void PathQuoteSpaces(
      System.Text.StringBuilder lpsz);
    #endregion

    #region PathRelativePathTo
    /// <summary>
    /// Creates a relative path from one file or folder to another
    /// </summary>
    /// <param name="path"></param>
    /// <param name="from"></param>
    /// <param name="attrFrom"></param>
    /// <param name="to"></param>
    /// <param name="attrTo"></param>
    /// <returns>Returns TRUE if successful, or FALSE otherwise</returns>
    [DllImport("shlwapi.dll", CharSet=CharSet.Auto)]
    static public extern bool PathRelativePathTo(
      System.Text.StringBuilder path,
      string from,
      int attrFrom,
      string to,
      int attrTo);
    #endregion

    #region PathRemoveArgs
    /// <summary>
    /// Removes any arguments from a given path
    /// </summary>
    /// <param name="path"></param>
    [DllImport("shlwapi.dll", CharSet=CharSet.Auto)]
    static public extern void PathRemoveArgs(
      System.Text.StringBuilder path);
    #endregion

    #region PathRemoveBackslash
    /// <summary>
    /// Removes the trailing backslash from a given path
    /// </summary>
    /// <param name="path"></param>
    /// <returns>Returns the address of the NULL that replaced the backslash, or the address of the last character if it's not a backslash</returns>
    [DllImport("shlwapi.dll", CharSet=CharSet.Auto)]
    static public extern string PathRemoveBackslash(
      string path);
    #endregion

    #region DllGetVersion

    [DllImport("shlwapi.dll", CharSet=CharSet.Auto)]
    static public extern int DllGetVersion( ref DLLVERSIONINFO2 path);

    #endregion

  } // class Shlwapi

  #endregion

  #region UxTheme

  /// <summary>
  /// Implements UxTheme calls
  /// </summary>
  public class UxTheme
  {
    [DllImport("UxTheme.dll", CharSet=CharSet.Auto)]
    static public extern IntPtr OpenThemeData(IntPtr hWnd, string pszClassList);

    [DllImport("UxTheme.dll", CharSet=CharSet.Auto)]
    static public extern int CloseThemeData(IntPtr hTheme);

    [DllImport("UxTheme.dll", CharSet=CharSet.Auto)]
    static public extern bool IsThemeActive();

    [DllImport("UxTheme.dll", CharSet=CharSet.Auto)]
    static public extern bool IsAppThemed();

    [DllImport("UxTheme.dll", CharSet=CharSet.Auto)]
    static public extern int DrawThemeBackground( IntPtr hWnd, IntPtr hDc, int iPartId, int iStateId,
      RECT pRect, RECT pClipRect);

    [DllImport("UxTheme.dll", CharSet=CharSet.Auto)]
    static public extern int GetCurrentThemeName( StringBuilder pszThemeFileName, int dwMaxNameChars,
      StringBuilder pszColorBuff, int cchMaxColorChars,
      StringBuilder pszSizeBuff, int cchMaxSizeChars);

  } // class UxTheme

  #endregion

  #region WndProc32
  /// <summary>
  /// Implements WndProc32 interfaces 
  /// </summary>
  public class WndProc32
  {
    [DllImport("user32.dll")]
    public static extern IntPtr SetWindowLong( IntPtr window,	int index, WndProc value);

    [DllImport("user32.dll")]
    public static extern IntPtr SetWindowLong( IntPtr window,	int index, GetWndProc value);

    [DllImport("user32.dll")]
    public static extern IntPtr SetWindowLong( IntPtr window,	int index, IntPtr value);

    [DllImport("user32.dll")]
    public static extern int GetWindowLong( IntPtr window, int index);

    [DllImport("user32.dll")]
    public static extern int CallWindowProc( IntPtr value, IntPtr Handle, int Msg, IntPtr WParam, IntPtr LParam);

    [DllImport("user32.dll")]
    public static extern int DefWindowProc(	IntPtr Handle, int Msg,	IntPtr WParam, IntPtr LParam);

    public delegate int GetWndProc(IntPtr hWnd, int Msg, IntPtr WParam, IntPtr LParam);

    public delegate void WndProc(ref System.Windows.Forms.Message m);

  } // class WndProc

  #endregion


} // namespace Win32

#region revision history

// $Log: win32.cs,v $
// Revision 1.14  2006/03/09 21:35:01  gdavidson
// Added SetWindowPlacement and AllowSetForegroundWindow
//
// Revision 1.13  2006/02/06 20:07:57  gdavidson
// Added enumerations and structures for LVCOLUMN
//
// Revision 1.12  2006/01/09 19:41:27  gdavidson
// Changed LVGROUP from class to struct
//
// Revision 1.11  2005/12/15 18:02:55  gdavidson
// Added missing LVM enumerations
//
// Revision 1.10  2005/11/23 03:59:42  geconomos
// viewer launching revisted
//
// Revision 1.9  2005/06/07 15:06:48  romy
// Added Enumns for DynamicListView
//
// Revision 1.8  2005/05/11 11:52:35  mkontak
// Added structs and interfaces
//
// Revision 1.7  2005/05/05 17:39:41  gdavidson
// Added SetFocus method.
//
// Revision 1.6  2001/01/02 12:43:27  gdavidson
// Added methods to retrieve images from the system image list.
//
// Revision 1.5  2004/11/17 22:34:02  geconomos
// added GetLastError
//
// Revision 1.4  2004/11/17 13:24:25  geconomos
// added uxtheme.dll interop
//
// Revision 1.3  2004/10/11 23:59:53  geconomos
// Initial "reskinning"
//
// Revision 1.2  2004/05/13 13:32:45  geconomos
// Added methods for rebar control.
//
// Revision 1.1.1.1  2004/03/02 03:58:38  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.10  2003/09/05 14:58:32  michael
// merged the branch "XP1_1" back into the trunc
//
// Revision 1.9  2003/08/20 14:23:25  michael
// added "mailto:"
//
// Revision 1.8.2.1  2003/08/21 21:20:44  mkontak
// Created a new folder browser
//
// Revision 1.8  2003/05/25 18:12:07  michael
// .net --> .com
//
// Revision 1.7  2003/04/29 13:08:40  geconomos
// Code reviews.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.UI/win32.cs,v 1.14 2006/03/09 21:35:01 gdavidson Exp $
// $Id: win32.cs,v 1.14 2006/03/09 21:35:01 gdavidson Exp $

#endregion
