program checkKind
implicit none
   integer :: i 
   real :: r 
   character :: c 
   logical :: lg 
   complex :: cp 
   
   print *,' Integer ', kind(i) 
   print *,' Real ', kind(r) 
   print *,' Complex ', kind(cp)
   print *,' Character ', kind(c) 
   print *,' Logical ', kind(lg)
   
end program checkKind
