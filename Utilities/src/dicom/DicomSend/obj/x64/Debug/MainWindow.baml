   M S B A M L   `   `   `  ����  	DicomSend4    .Viatronix.Utilities.Dicom.DicomSend.MainWindow   5   	   R NWindowsBase, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35W SPresentationCore, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35\ XPresentationFramework, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35D 9http://schemas.microsoft.com/winfx/2006/xaml/presentation    5      R NSystem.Xaml, Version=4.0.0.0, Culture=neutral, PublicKeyToken=b77a5c561934e089:x,http://schemas.microsoft.com/winfx/2006/xaml     5      -   5          � Title$  
Dicom Send��5      $	��609��6   $	��609��6"   $	��436��61   $	��436��6>   
����İ�.��5      � .+�5      �� $��
Directory:��6   
����  �$
��Bold"�67   $��28��6I   $
��Left=�6U   $��	15,13,0,0q�6p   $	��Top=�6�   �� 5      -   5         ��Name$ m_directoryText��5      $��23��6�   $��159,15,60,0q�6�   $	��Top=�6�   $��1��6�    � ToolTip$0 *Specifies the directory to be transferred.��6�   ��6'   �� *	Directory+ �� Mode$ TwoWay=� �� UpdateSourceTrigger$ PropertyChanged=�{�6t   �� *IsNotActive+$ OneWay=�6�   �� 5      $	��...��6   $��23��6I   $��Right=�6U   $��	0,15,15,0q�6q   $	��Top=�6�   $��31��6�   
����  �$
��Bold"�6�   $��2��6�   $F @Displays a dialog to assist in picking a directory for transfer.��6�   ��6   �� *DirectorySelectionCommand+6�   �� 5
      $��Filter:��6   
����  �$
��Bold"�64   $��28��6F   $
��Left=�6R   $��	15,48,0,0q�6m   $	��Top=�6�   �� 5      -   5        $ m_filterText��5      $��23��6�   $��159,50,15,0q�6�   $	��Top=�6�   $��3��6�   $$ Determines what files to send.��6�   ��6$   �� *Filter+$ TwoWay=�$ PropertyChanged=�{�6n   �� *IsNotActive+$ OneWay=�6�   �� 5      -   5        $��label1��5   �   $��Application AE Title:��6   
����  �$
��Bold"�6B   $��28��6T   $
��Left=�6`   $��	15,83,0,0q�6{   $	��Top=�6�   �� 5      -   5        $ m_applicationAETitle��5      $��23��6�   $��159,85,15,0q�6�   $	��Top=�6�   $��5��6�   $: 4AETitle of the DICOM device performing the transfer.��6�   ��6,   �� *	AETitle+$ TwoWay=�$ PropertyChanged=�{�6w   �� *IsNotActive+$ OneWay=�6�   �� 5      $��Transfer Syntax:��6   
����  �$
��Bold"�6=   $��28��6O   $
��Left=�6[   $��
15,118,0,0q�6v   $	��Top=�6�   �� 5      $��23��6k   $��159,120,15,0q�6w   $	��Top=�6�   $��6��6�   $ The mode of transfer.��6�    �� Text 6   �� *TransferSyntax+$ TwoWay=�{�6<   �� *IsNotActive+$ OneWay=�@�5      �� 5   
   $
��Both��6   �� 5   
   $��Implicit��6   �� 5   
   $��Explicit��6   5      �� 5      $��Destination Server:��6   
����  �$
��Bold"�6@   $��
15,158,0,0q�6R   $
��Left=�6f   $	��117��6�   $��24��6�   $	��Top=�6�   �� 5      -   5        $ m_destinationServerText��5      $��23��6�   $��159,160,15,0q�6�   $	��Top=�6�   $��7��6�   $0 *Specifies the destination of the transfer.��6�   ��6/   �� *Server+$ TwoWay=�$ PropertyChanged=�{�6y   �� *IsNotActive+$ OneWay=�6�   �� 5      $��Destination Port:��6   
����  �$
��Bold"�6>   $��28��6P   $
��Left=�6\   $��
15,193,0,0q�6w   $	��Top=�6�   �� 5      -   5        $ m_destinationPortText��5      $��23��6�   $��159,195,15,0q�6�   $	��Top=�6�   $��8��6�   $&  Destination DICOM Receiver Port.��6�   ��6-   �� *Port+$ TwoWay=�$ PropertyChanged=�{�6u   �� *IsNotActive+$ OneWay=�6�   �� 5      $��Destination AE Title:��6   
����  �$
��Bold"�6B   $��28��6T   $
��Left=�6`   $��
15,228,0,0q�6{   $	��Top=�6�   �� 5       -   5        $ m_destinationAETitleText��5       $��23��6�   $��159,230,15,0q�6�   $	��Top=�6�   $��9��6�   $E ?AETitle of the DICOM device accepting delivery of the transfer.��6�   ��60   �� *CallingAETitle+$ TwoWay=�$ PropertyChanged=�{�6�   �� *IsNotActive+$ OneWay=�6�   �� 5(      -	   5         ��Name$ m_threadsListBox��5(      $��15,273,15,59q�6(   $��Stretch=�6^   $��Disabled=�6�   $��10��6�   $/ )Current status of the transfer operation.��6�   ��6>   �� *	Senders+��5)      �� 5*   
   .R�5+      �� ��6   �� *ViewportWidth+ �� RelativeSource �� *FindAncestor+ �� AncestorType" ��.+�5,      �� 
����d�$��2q�62   $��3,3,3,3q�6F   ._�5-      � $��3,3,3,3q�6   	 � ColumnDefinitions	 5.      �� 5/      �� 50      
 � RowDefinitions
 52      �� 53      �� 54      $��3 �6"   �� 55      .+�58      �� $��0��6=   $��0��6J   ��6   �� *ID+$ OneWay=�6J   � 5:       � Minimum$ 0Q�6    � Maximum$	 100Q�6*   $��15��68   $��0��6l   $��1��6y    � Value 6D   �� *
Progress+$ OneWay=���5;      �� 5<       �� Triggers 5=      �� 5>       �� Binding 6&   �� *Status+ �� Value 	Completed6A   .a�5?      ��  �� Property � y� �� Value$ Hidden=�6E   5@      �� 5A       6&   �� *Status+
 Failed6A   .a�5B      ��  � y�$ Hidden=�6E   5C      �� 5D       6&   �� *Status+ Pending6A   .a�5E      ��  � y�$ Visible=�6E   5F      5H      5J      �� 5L      $��2��6D   $��0��6Q   ��6   �� *	Directory+$ OneWay=�6Q   �� 5N       ��Name$ m_statusText��6   $��0��62   $��1��6?    �� TextAlignment$ Right=�6O   $
��Bold"�6e   ��5O      �� 5P       5Q      �� 5R       6&   �� *Status+ 	Completed6A   .a�5S      ��  � y�$ Visible=�6C   �� 5T       � ��$ Completed Successfully.��6=   �� 5U       � �� �#00a9006C   5V      �� 5W       6&   �� *Status+
 Failed6A   .a�5X      ��  � y�$ Visible=�6C   �� 5Y       � ��$/ )Send Failed.  Please see log for details.��6=   �� 5Z       � �� �#e900006C   5[      �� 5\       6&   �� *Status+ Pending6A   .a�5]      ��  � y�$ Hidden=�6C   5^      5`      5b      �� 5d      $ m_messageText��6   $��2��6Y   $��1��6f   $ Right=�6v   ��63   �� *	Message+$ OneWay=���5e      �� 5f       5g      �� 5h       6&   �� *Status+$ OneWay=�
 Failed6N   .a�5i      ��  � �� �#e900006C   �� 5j       � ��$
 Bold"�6C   5k      �� 5l       6&   �� *Status+$ OneWay=� Pending6N   .a�5m      ��  � �� �#0000006C   �� 5n       � ��$ Normal"�6C   5o      5q      5s      5u      5v      5w      5x      5z      �� 5|      $��Start��6   
����  �$
��Bold"�6T   $��	0,0,85,20q�6f   $��Right=�6y   $��65��6�    �� 	IsDefault . $��11��6�   $��25��6�   $��Bottom=�6�   ��6   �� *StartCommand+6�   �� 5}      $��Cancel��6   
����  �$
��Bold"�6U   $��Right=�6g   $��	0,0,15,20q�6�   $��65��6�    �� IsCancel . $��12��6�   $��25��6�   $��Bottom=�6�   ��6   �� *AbortCommand+6�   5      5�      