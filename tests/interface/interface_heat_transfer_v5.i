[Mesh]
  type = GeneratedMesh
  dim = 2
  nx = 12
  xmax = 12
  ymax = 3
  zmax = 3
[]

[Variables]
  [./temp]
  [../]
[]

[Kernels]
  [./hc]
    type = HeatConduction
    variable = temp
  [../]
  [./td]
    type = TimeDerivative
    variable = temp
  [../]
[]

[BCs]
  [./left]
    type = DirichletBC
    variable = temp
    boundary = left
    value = 400
  [../]
  [./right]
    type = DirichletBC
    variable = temp
    boundary = right
    value = 300
  [../]
[]

[Materials]
  [./hcm]
    type = HeatConductionMaterial
    block = 0
    specific_heat = 1
    thermal_conductivity = 1
  [../]
[]

[Executioner]
  type = Transient
  num_steps = 9
  solve_type = PJFNK
  petsc_options_iname = '-pc_type -pc_hypre_type'
  petsc_options_value = 'hypre boomeramg'
[]

[Outputs]
  [./out]
    type = Exodus
  [../]
[]

