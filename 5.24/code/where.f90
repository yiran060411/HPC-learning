program whereStatement
implicit none
   integer :: a(3,5), i , j
   
   do i = 1,3
      do j = 1, 5                
         a(i,j) = j-i          
      end do 
   end do
   
   Print *, 'The A array:'
   
   do i = lbound(a,1), ubound(a,1)
      write(*,*) (a(i,j), j = lbound(a,2), ubound(a,2))
   end do
   
   where( a<0 ) 
      a = 1 
   elsewhere
      a = 5
   end where
  
   Print *, 'The A array:'
   do i = lbound(a,1), ubound(a,1)
      write(*,*) (a(i,j), j = lbound(a,2), ubound(a,2))
   end do   
   
end program whereStatement
