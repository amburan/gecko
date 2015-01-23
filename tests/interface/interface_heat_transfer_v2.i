[Mesh]
  type = GeneratedMesh
  dim = 2
  nx = 12
  xmax = 24
  ymax = 6
  zmax = 6
[]

[MeshModifiers]
  active = 'interface right'
  [./left]
    type = SubdomainBoundingBox
    bottom_left = '0 0 0'
    top_right = '4 5 0'
    block_name = left
    block_id = 1
  [../]
  [./interface]
    type = SubdomainBoundingBox
    bottom_left = '4 0 0'
    top_right = '6 5 0'
    block_name = interface
    block_id = 1
  [../]
  [./right]
    type = SubdomainBoundingBox
    bottom_left = '6 0 0'
    top_right = '10 5 0'
    block_name = right
    block_id = 2
  [../]
[]

[Variables]
  [./t]
  [../]
[]

[Kernels]
  [./diff]
    type = MultiscaleHeatConduction
    variable = t
  [../]
[]

[BCs]
  [./left]
    type = DirichletBC
    variable = t
    boundary = left
    value = 1
  [../]
  [./right]
    type = DirichletBC
    variable = t
    boundary = right
    value = 0
  [../]
[]

[Materials]
  [./left_mat]
    type = HeatConductionMaterial
    block = 0
    specific_heat = 1
    thermal_conductivity = 1
  [../]
  [./interface]
    type = HeatConductionMaterial
    block = 1
    specific_heat = 1
    thermal_conductivity = 0.5
  [../]
  [./right]
    type = HeatConductionMaterial
    block = 2
    specific_heat = 1
    thermal_conductivity = 1
  [../]
[]

[Executioner]
  type = Transient
[]

