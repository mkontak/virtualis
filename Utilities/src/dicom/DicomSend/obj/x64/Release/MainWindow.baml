   M S B A M L   `   `   `  ����   	DicomSend4    .Viatronix.Utilities.Dicom.DicomSend.MainWindow   R NWindowsBase, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35W SPresentationCore, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35\ XPresentationFramework, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35D 9http://schemas.microsoft.com/winfx/2006/xaml/presentation    R NSystem.Xaml, Version=4.0.0.0, Culture=neutral, PublicKeyToken=b77a5c561934e089:x,http://schemas.microsoft.com/winfx/2006/xaml     -     � Title$  
Dicom Send��$	��609��$	��609��$	��436��$	��436��
����İ�.��� .+��� $��
Directory:��
����  �$
��Bold"�$��28��$
��Left=�$��	15,13,0,0q�$	��Top=��� -    ��Name$ m_directoryText��$��23��$��159,15,60,0q�$	��Top=�$��1�� � ToolTip$0 *Specifies the directory to be transferred.������ *	Directory+ �� Mode$ TwoWay=� �� UpdateSourceTrigger$ PropertyChanged=�{��� *IsNotActive+$ OneWay=��� $	��...��$��23��$��Right=�$��	0,15,15,0q�$	��Top=�$��31��
����  �$
��Bold"�$��2��$F @Displays a dialog to assist in picking a directory for transfer.������ *DirectorySelectionCommand+�� $��Filter:��
����  �$
��Bold"�$��28��$
��Left=�$��	15,48,0,0q�$	��Top=��� -   $ m_filterText��$��23��$��159,50,15,0q�$	��Top=�$��3��$$ Determines what files to send.������ *Filter+$ TwoWay=�$ PropertyChanged=�{��� *IsNotActive+$ OneWay=��� -   $��label1��$��Application AE Title:��
����  �$
��Bold"�$��28��$
��Left=�$��	15,83,0,0q�$	��Top=��� -   $ m_applicationAETitle��$��23��$��159,85,15,0q�$	��Top=�$��5��$: 4AETitle of the DICOM device performing the transfer.������ *	AETitle+$ TwoWay=�$ PropertyChanged=�{��� *IsNotActive+$ OneWay=��� $��Transfer Syntax:��
����  �$
��Bold"�$��28��$
��Left=�$��
15,118,0,0q�$	��Top=��� $��23��$��159,120,15,0q�$	��Top=�$��6��$ The mode of transfer.�� �� Text �� *TransferSyntax+$ TwoWay=�{��� *IsNotActive+$ OneWay=�@��� $
��Both���� $��Implicit���� $��Explicit���� $��Destination Server:��
����  �$
��Bold"�$��
15,158,0,0q�$
��Left=�$	��117��$��24��$	��Top=��� -   $ m_destinationServerText��$��23��$��159,160,15,0q�$	��Top=�$��7��$0 *Specifies the destination of the transfer.������ *Server+$ TwoWay=�$ PropertyChanged=�{��� *IsNotActive+$ OneWay=��� $��Destination Port:��
����  �$
��Bold"�$��28��$
��Left=�$��
15,193,0,0q�$	��Top=��� -   $ m_destinationPortText��$��23��$��159,195,15,0q�$	��Top=�$��8��$&  Destination DICOM Receiver Port.������ *Port+$ TwoWay=�$ PropertyChanged=�{��� *IsNotActive+$ OneWay=��� $��Destination AE Title:��
����  �$
��Bold"�$��28��$
��Left=�$��
15,228,0,0q�$	��Top=��� -   $ m_destinationAETitleText��$��23��$��159,230,15,0q�$	��Top=�$��9��$E ?AETitle of the DICOM device accepting delivery of the transfer.������ *CallingAETitle+$ TwoWay=�$ PropertyChanged=�{��� *IsNotActive+$ OneWay=��� -	    ��Name$ m_threadsListBox��$��15,273,15,59q�$��Stretch=�$��Disabled=�$��10��$/ )Current status of the transfer operation.������ *	Senders+���� .R��� ���� *ViewportWidth+ �� RelativeSource �� *FindAncestor+ �� AncestorType" ��.+��� 
����d�$��2q�$��3,3,3,3q�._�� $��3,3,3,3q�	 � ColumnDefinitions	 �� �� 
 � RowDefinitions
 �� �� $��3 ��� .+��� $��0��$��0������ *ID+$ OneWay=��  � Minimum$ 0Q� � Maximum$	 100Q�$��15��$��0��$��1�� � Value �� *
Progress+$ OneWay=�����  �� Triggers ��  �� Binding �� *Status+ �� Value 	Completed.a���  �� Property � y� �� Value$ Hidden=���  �� *Status+
 Failed.a���  � y�$ Hidden=���  �� *Status+ Pending.a���  � y�$ Visible=��� $��2��$��0������ *	Directory+$ OneWay=���  ��Name$ m_statusText��$��0��$��1�� �� TextAlignment$ Right=�$
��Bold"�����  ��  �� *Status+ 	Completed.a���  � y�$ Visible=���  � ��$ Completed Successfully.����  � �� �#00a900��  �� *Status+
 Failed.a���  � y�$ Visible=���  � ��$/ )Send Failed.  Please see log for details.����  � �� �#e90000��  �� *Status+ Pending.a���  � y�$ Hidden=��� $ m_messageText��$��2��$��1��$ Right=����� *	Message+$ OneWay=�����  ��  �� *Status+$ OneWay=�
 Failed.a���  � �� �#e90000��  � ��$
 Bold"���  �� *Status+$ OneWay=� Pending.a���  � �� �#000000��  � ��$ Normal"��� $��Start��
����  �$
��Bold"�$��	0,0,85,20q�$��Right=�$��65�� �� 	IsDefault . $��11��$��25��$��Bottom=����� *StartCommand+�� $��Cancel��
����  �$
��Bold"�$��Right=�$��	0,0,15,20q�$��65�� �� IsCancel . $��12��$��25��$��Bottom=����� *AbortCommand+