[Mesh]
  type = GeneratedMesh
  dim = 2
  nx = 12
  xmax = 24
  ymax = 6
  zmax = 6
[]

[Variables]
  [./temp]
  [../]
[]

[Kernels]
  [./hc]
    type = MultiscaleHeatConduction
    variable = temp
  [../]
[]

[BCs]
  [./left]
    type = DirichletBC
    variable = temp
    boundary = left
    value = 1
  [../]
  [./right]
    type = DirichletBC
    variable = temp
    boundary = right
    value = 0
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
[]
