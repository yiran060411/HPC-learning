program factorial  
implicit none  
   ! define variables
   integer :: nfact = 1   
   integer :: n  
   
   ! compute factorials   
   do n = 1, 10      
      nfact = nfact * n 
      ! print values
      print*,  n, " ", nfact   
   end do 
   
end program factorial  
