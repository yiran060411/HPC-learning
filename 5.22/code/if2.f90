program markGradeA  
implicit none  
   real :: marks
   ! assign marks   
   marks = 90.4
   ! use an if statement to give grade
  
   gr: if (marks > 90.0) then  
   print *, " Grade A"
   end if gr
end program markGradeA 
