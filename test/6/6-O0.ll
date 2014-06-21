; ModuleID = '6-O0.bc'
target datalayout = "e-p:64:64:64-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-v64:64:64-v128:128:128-a0:0:64-s0:64:64-f80:128:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@main.a = private unnamed_addr constant [3 x [3 x i32]] [[3 x i32] [i32 1, i32 2, i32 3], [3 x i32] [i32 4, i32 5, i32 6], [3 x i32] [i32 7, i32 8, i32 9]], align 16
@main.b = private unnamed_addr constant [3 x [3 x i32]] [[3 x i32] [i32 2, i32 3, i32 4], [3 x i32] [i32 5, i32 6, i32 7], [3 x i32] [i32 9, i32 1, i32 8]], align 16

; Function Attrs: nounwind uwtable
define i32 @main() #0 {
  %1 = alloca i32, align 4
  %a = alloca [3 x [3 x i32]], align 16
  %b = alloca [3 x [3 x i32]], align 16
  %c = alloca [3 x [3 x i32]], align 16
  %i = alloca i32, align 4
  %j = alloca i32, align 4
  %k = alloca i32, align 4
  store i32 0, i32* %1
  %2 = bitcast [3 x [3 x i32]]* %a to i8*
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* %2, i8* bitcast ([3 x [3 x i32]]* @main.a to i8*), i64 36, i32 16, i1 false)
  %3 = bitcast [3 x [3 x i32]]* %b to i8*
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* %3, i8* bitcast ([3 x [3 x i32]]* @main.b to i8*), i64 36, i32 16, i1 false)
  %4 = bitcast [3 x [3 x i32]]* %c to i8*
  call void @llvm.memset.p0i8.i64(i8* %4, i8 0, i64 36, i32 16, i1 false)
  store i32 0, i32* %i, align 4
  br label %5

; <label>:5                                       ; preds = %48, %0
  %6 = load i32* %i, align 4
  %7 = icmp slt i32 %6, 3
  br i1 %7, label %8, label %51

; <label>:8                                       ; preds = %5
  store i32 0, i32* %j, align 4
  br label %9

; <label>:9                                       ; preds = %44, %8
  %10 = load i32* %j, align 4
  %11 = icmp slt i32 %10, 3
  br i1 %11, label %12, label %47

; <label>:12                                      ; preds = %9
  store i32 0, i32* %k, align 4
  br label %13

; <label>:13                                      ; preds = %40, %12
  %14 = load i32* %k, align 4
  %15 = icmp slt i32 %14, 3
  br i1 %15, label %16, label %43

; <label>:16                                      ; preds = %13
  %17 = load i32* %k, align 4
  %18 = sext i32 %17 to i64
  %19 = load i32* %i, align 4
  %20 = sext i32 %19 to i64
  %21 = getelementptr inbounds [3 x [3 x i32]]* %a, i32 0, i64 %20
  %22 = getelementptr inbounds [3 x i32]* %21, i32 0, i64 %18
  %23 = load i32* %22, align 4
  %24 = load i32* %j, align 4
  %25 = sext i32 %24 to i64
  %26 = load i32* %k, align 4
  %27 = sext i32 %26 to i64
  %28 = getelementptr inbounds [3 x [3 x i32]]* %b, i32 0, i64 %27
  %29 = getelementptr inbounds [3 x i32]* %28, i32 0, i64 %25
  %30 = load i32* %29, align 4
  %31 = mul nsw i32 %23, %30
  %32 = load i32* %j, align 4
  %33 = sext i32 %32 to i64
  %34 = load i32* %i, align 4
  %35 = sext i32 %34 to i64
  %36 = getelementptr inbounds [3 x [3 x i32]]* %c, i32 0, i64 %35
  %37 = getelementptr inbounds [3 x i32]* %36, i32 0, i64 %33
  %38 = load i32* %37, align 4
  %39 = add nsw i32 %38, %31
  store i32 %39, i32* %37, align 4
  br label %40

; <label>:40                                      ; preds = %16
  %41 = load i32* %k, align 4
  %42 = add nsw i32 %41, 1
  store i32 %42, i32* %k, align 4
  br label %13

; <label>:43                                      ; preds = %13
  br label %44

; <label>:44                                      ; preds = %43
  %45 = load i32* %j, align 4
  %46 = add nsw i32 %45, 1
  store i32 %46, i32* %j, align 4
  br label %9

; <label>:47                                      ; preds = %9
  br label %48

; <label>:48                                      ; preds = %47
  %49 = load i32* %i, align 4
  %50 = add nsw i32 %49, 1
  store i32 %50, i32* %i, align 4
  br label %5

; <label>:51                                      ; preds = %5
  ret i32 0
}

; Function Attrs: nounwind
declare void @llvm.memcpy.p0i8.p0i8.i64(i8* nocapture, i8* nocapture readonly, i64, i32, i1) #1

; Function Attrs: nounwind
declare void @llvm.memset.p0i8.i64(i8* nocapture, i8, i64, i32, i1) #1

attributes #0 = { nounwind uwtable "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind }

!llvm.ident = !{!0}

!0 = metadata !{metadata !"Ubuntu clang version 3.4-1ubuntu3 (tags/RELEASE_34/final) (based on LLVM 3.4)"}
